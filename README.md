# C-Shell

Implemented a shell from scratch, using modular C code, with commands and features like IO redirection, piping, auto-completion, background and foreground processes, signals, stored command history etc

# C-Shell Guide

## 1. Basic Usage
- Prompt format: `<Username@SystemName:~>`
- Supports ';' and '&' separated commands
- Handles random spaces and tabs in input

## 2. Built-in Commands

### warp
Change directory (supports '.', '..', '~', '-')

### peek
List files and directories
- Flags: `-a` (all files), `-l` (detailed view)

### pastevents
- Show command history (15 most recent)
- `pastevents purge`: Clear history
- `pastevents execute <index>`: Execute command from history

### proclore
Display process information

### seek
Search for files/directories
- Flags: `-d` (directories only), `-f` (files only), `-e` (execute/enter if single result)

### activities
List running processes spawned by the shell

### ping <pid> <signal_number>
Send signals to processes

### fg <pid> / bg <pid>
Manage background processes

## 3. I/O Redirection
Supports `>`, `<`, and `>>` operators

## 4. Piping
Supports multiple pipes (`|`)

## 5. Signal Handling
- `Ctrl-C`: Interrupt foreground process
- `Ctrl-D`: Log out of shell
- `Ctrl-Z`: Push foreground process to background

## 6. Additional Features
- Background process management
- Execution time display for processes >2 seconds
- Color-coded output for files/directories
- `iMan <command_name>`: Fetch man pages from the internet

## 7. Error Handling
- Displays appropriate error messages
- Handles invalid commands and arguments

## Compiling and Running
1. Use `make -b` command to compile
2. Run `./a.out` to start the shell
