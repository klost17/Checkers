# Checkers
Artificial Intelligence capable of playing checkers at high level

# Compile
g++ *.cpp -Wall -o checkers

# Run
# The players use standard input and output to communicate
# The Moves made are shown as unicode-art on std err if the parameter verbose is given

# Play against self in same terminal
mkfifo pipe
./checkers init verbose < pipe | ./checkers > pipe

# Play against self in two different terminals
# Terminal 1:
mkfifo pipe1 pipe2
./checkers init verbose < pipe1 > pipe2

# Terminal 2:
./checkers verbose > pipe1 < pipe2
