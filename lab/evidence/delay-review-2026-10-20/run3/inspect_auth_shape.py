from pathlib import Path
import importlib.util

base = Path(__file__).resolve().parent
repo = base.parents[3]
spec = importlib.util.spec_from_file_location('ber_test', repo / 'apps/ccli/tests/unit/mms_presentation_context_test.py')
ber = importlib.util.module_from_spec(spec)
spec.loader.exec_module(ber)

def show(nodes, path=''):
    for index, (tag, value) in enumerate(nodes):
        here = path + '/' + str(index) + ':' + hex(tag)
        if isinstance(value, list):
            print(here, 'constructed')
            show(value, here)
        else:
            print(here, 'length', len(value), 'value', value.hex() if len(value) < 32 else '<omitted>')

show(ber.decode((base / 'captured-presentation.bin').read_bytes()))
