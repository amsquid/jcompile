
  # jcompile
  
Basic compiler written in C<br>
Takes a .txc file, converts it into assembly, then converts that file into an executable file.


  ## Installation
  
Clone the project with:<br>
```bash
git clone https://github.com/amsquid/jcompile.git
```
Compile with:<br>
```bash
./build.sh
```
... and run jcompile with a filename as the first argument:
```bash
./jcompile test.txc
```

  ## Requirements

GCC and NASM

  ## Usage
A txc program has a pointer that points to anywhere on a 512 byte array.<br>
The program has the following instructions:
```txt
+   Adding the value at the current array pointer by 1
-   Subtracting the value at the current array pointer by 1
>   Increments the pointer by 1
<   Decrements the pointer by 1
@   Prints the value at the pointer as a character
#   Prints the value at the pointer as a number
```


  ## License
  
  [MIT](https://choosealicense.com/licenses/mit/)
