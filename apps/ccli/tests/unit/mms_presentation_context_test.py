"""Regression for TSP context 5, legacy clients, and generated BER lengths.

Run with a compiled mms_presentation_context_probe and the captured TSP CP:
  python3 mms_presentation_context_test.py PROBE captured-presentation.bin
The capture remains local evidence; no credentials are needed for these tests.
"""
from pathlib import Path
import subprocess
import sys
import tempfile

AUTH = bytes.fromhex('2886480001010401')
LEGACY = bytes.fromhex('288648000001010401')


def decode(data):
    """Strict definite-length BER walker, independent of libIEC61850."""
    result, pos = [], 0
    while pos < len(data):
        tag = data[pos]
        pos += 1
        assert tag & 31 != 31, 'high-tag encoding outside this fixture'
        length = data[pos]
        pos += 1
        if length & 128:
            count = length & 127
            assert 0 < count <= 4 and pos + count <= len(data)
            length = int.from_bytes(data[pos:pos + count], 'big')
            pos += count
        assert pos + length <= len(data), 'BER length extends beyond container'
        value = data[pos:pos + length]
        pos += length
        result.append((tag, decode(value) if tag & 32 else value))
    assert pos == len(data)
    return result


def encode(nodes):
    result = b''
    for tag, value in nodes:
        value = encode(value) if isinstance(value, list) else value
        length = len(value)
        raw = length.to_bytes(max(1, (length.bit_length() + 7) // 8), 'big')
        result += bytes([tag]) + (bytes([length]) if length < 128 else bytes([128 | len(raw)]) + raw) + value
    return result


def replace_oid(nodes, old, new):
    return [(tag, replace_oid(value, old, new) if isinstance(value, list)
             else new if tag == 6 and value == old else value) for tag, value in nodes]


def contexts(nodes):
    return next(value for tag, value in nodes[0][1] if tag == 0xa2)


def main():
    probe, capture = map(Path, sys.argv[1:])
    original = capture.read_bytes()
    tree = decode(original)
    assert AUTH in original and LEGACY not in original
    with tempfile.TemporaryDirectory(prefix='ccli-presentation-test-') as temporary:
        work = Path(temporary)

        def check(name, data, accepted):
            path = work / (name + '.bin')
            path.write_bytes(data)
            result = subprocess.run([str(probe), 'parse', str(path)], capture_output=True, text=True)
            assert result.returncode == (0 if accepted else 1), (name, result.stdout, result.stderr)
            print(name + ': PASS ' + result.stdout.strip())

        check('unchanged-TSP-capture', original, True)
        check('legacy-extra-zero-alias', encode(replace_oid(tree, AUTH, LEGACY)), True)
        check('unknown-auth-syntax-rejected', encode(replace_oid(tree, AUTH, AUTH[:-1] + b'\x02')), False)
        two = decode(original)
        definition = next(value for tag, value in contexts(two) if tag == 0xa4)
        del definition[2]
        check('two-context-client', encode(two), True)
        four = decode(original)
        definition = next(value for tag, value in contexts(four) if tag == 0xa4)
        definition.append(definition[2])
        check('four-contexts-rejected', encode(four), False)
        for mechanism, count in [('cert', 3), ('tls', 2)]:
            output = work / (mechanism + '.bin')
            subprocess.run([str(probe), 'encode-' + mechanism, str(output)], check=True)
            encoded = output.read_bytes()
            generated = decode(encoded)  # Every constructed BER length must fit exactly.
            definition = next(value for tag, value in contexts(generated) if tag == 0xa4)
            assert len(definition) == count
            assert (AUTH in encoded) == (mechanism == 'cert') and LEGACY not in encoded
            check('generated-' + mechanism + '-roundtrip-and-BER-lengths', encoded, True)


if __name__ == '__main__':
    main()
