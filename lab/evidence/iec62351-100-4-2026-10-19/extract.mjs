import fs from 'node:fs';
import {getDocument} from 'file:///C:/Users/Administrator/.cache/codex-runtimes/codex-primary-runtime/dependencies/node/node_modules/pdfjs-dist/legacy/build/pdf.mjs';
const pdf=await getDocument({data:new Uint8Array(fs.readFileSync(process.argv[2])),useSystemFonts:true}).promise;
let text='';
for(let i=1;i<=pdf.numPages;i++) {
 const p=await pdf.getPage(i); const c=await p.getTextContent();
 text+='\n=== PDF PAGE '+i+' ===\n'+c.items.map(x=>x.str+(x.hasEOL?'\n':' ')).join('');
}
fs.writeFileSync(process.argv[3],text);
console.log('Extracted '+pdf.numPages+' pages');
