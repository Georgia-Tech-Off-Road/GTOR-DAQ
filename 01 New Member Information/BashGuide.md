# Guide
Bash is a shell scripting language, it is the most common language of commands that we use to interact with a terminal. But why do you need to learn bash? If you are a programmer it is imperative to learn bash. Many commands are only available from the command line, or have more functionality that way (i.e git). Additionally, because bash commands are just text, there is a lot of powerful scripting potential! Below are some common very basic bash commands, for a more thorough introduction consult [this resource](https://github.com/RehanSaeed/Bash-Cheat-Sheet):

## Common Commands
``ls`` -  (list) List all files and directories (folders) in your current directory (folder)
``pwd`` - (print working directory) prints the path to your current directory (folder)
``cd [dir_name]`` - (change directory) switches directory to ``dir_name``. E.x if you are in ``home/documents``, executing cd ``programming`` moves your current directory to ``home/documents/programming``.
``cd ..`` - Moves you to the parent directory of your current directory. E.x if you are in ``home/documents/programming``, executing ``cd ..`` changes your current working directory to ``home/documents``.
``vim [file_name]`` - Opens up the vim text editor. If [file_name] already exists, opens that file. If [file_name] does not exist, executing the command create a new file and opens the editor. Vim is its own topic, here's a good [cheatsheet](https://vim.rtorr.com/) for it.
``cat [file_name]`` - Prints the contents of [file_name]

These commands will get you quite far to becoming comfortable with the commmand line! As a general tip, typing ``--help`` after most commands will display a detailed message on the commands e.x ``cd --help``, ``ls --help``.
