```
swig -c++ -java -package com.linkedin.kytea -outdir com/linkedin/kytea mykytea.i


g++ -shared mykytea.o mykytea_wrap.o -o _LiKytea.so -Wl,-Bstatic -L/usr/local/include -l:libkytea.a  -fPIC  -Wl,-Bdynamic --as-needed 

Mykytea myKytea = new Mykytea("--model " + "your_model.bin");
```
