# Place offline Online Help HTML here (from Test Suite Pro install)

**Path:** `knowledge-base/08-engineering/testsuite-pro/help-offline/`  
**Status:** **HAVE** (2026-09-26) — mirrored from lab PC install

## Lab PC source (authoritative)

```text
C:\Program Files\Triangle MicroWorks\TMW Test Suite Pro\Help\
  Default.htm
  TMW_Print.pdf
  Content\ …
```

Executable: `C:\Program Files\Triangle MicroWorks\TMW Test Suite Pro\Bin\TestSuite.exe`  
FileVersion: **4.7.4.5037**

## Refresh after TSP upgrade

```powershell
robocopy "C:\Program Files\Triangle MicroWorks\TMW Test Suite Pro\Help" `
  "c:\Yahia\projects\CCLI\knowledge-base\08-engineering\testsuite-pro\help-offline" /E
powershell -File c:\Yahia\projects\CCLI\scripts\ingest-testsuite-pro.ps1
```

**Do not commit** the commercial installer. Help HTML/PDF is for internal RAG only.
