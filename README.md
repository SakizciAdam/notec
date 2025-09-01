# notec

Notec is a lightweight, console-based text editor for Windows.

## Features
- Simple and fast text editing
- Syntax highlighting 
- ~100kB size
- No bloat

## Installation

### Windows

```bash
git clone https://github.com/SakizciAdam/notec.git
cd notec
make
./notec.exe <pathtofile> [-r/-v]
```

### Linux

```bash
git clone https://github.com/SakizciAdam/notec.git
cd notec
make
make install // sudo make install for system wide installation
notec <pathtofile> [-r/-v]
```

## Requirements

### Windows

- MinGW (Build requirement)

### Linux
 
- xclip (For copy and paste)
- gcc (Build requirement)
- curses
- tinfo


## TODO

1. ~~Copy/paste~~ 

2. Replace
   
3. ~~Select text~~ 

4. Undo/redo

5. Configuration

## Contributing

Pull requests are welcome. For major changes, please open an issue first
to discuss what you would like to change.

Please make sure to update tests as appropriate.

## License

[MIT](https://choosealicense.com/licenses/mit/)