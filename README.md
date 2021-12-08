# Area Efficiency Model for Turbo-Decoders

## Build and Run

### Windows (MinGW64):
- Comment the Linux commands and uncomment the Windows ones in the makefile
```
# Linux
#	rm -f $(BINDIR)/main $(OBJDIR)/*.o
# Windows
	del /f .\bin\main.exe
	del /f /s .\obj\*.o
```
- If not present, create the directories ```.\obj\``` and ```.\bin\``` in the project directory
- Run ```mingw32-make.exe``` in the project directory
- Run ```.\bin\main.exe .\dat\input_data.txt```
- Clean: run ```mingw32-make.exe clean``` in the project directory

### Linux:
- Comment the Windows commands and uncomment the Linux ones in the makefile
```
# Linux
	rm -f $(BINDIR)/main $(OBJDIR)/*.o
# Windows
#	del /f .\bin\main.exe
#	del /f /s .\obj\*.o
```
- If not present, create the directories ```./obj/``` and ```./bin/``` in the project directory
- Run ```make``` in the project directory
- Run ```./bin/main ./dat/input_data.txt```
- Clean: run ```make clean``` in the project directory

