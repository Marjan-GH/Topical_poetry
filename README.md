# Topical Poetry Generation - Making Topical Finite State Machine


This is the source code for the "Generating Topical Poetry" (M. Ghazvininejad, X. Shi, Y. Choi, and K. Knight), Proc. EMNLP, 2016 paper.

Given a topic word or phrase, this toolkit generates a list of related words to the topic, chooses rhyme words and constructs an FSA of all possible poems in iambic pentameter format with appropriate rhythm and rhymes. 


## Getting Started
These instructions will get you a copy of the project and provide instruction for running the program. 

- First clone the github
- Download word2ved and distance files from https://drive.google.com/drive/folders/1LsygG2sQKlFJDXpz2es_NvXSC0t-NSDB?usp=sharing and place them in data folder


## To run

```
sh initial.sh
```

To compile all the files.

```
sh initial_server_1.sh &
```

Wait until it outputs "ready to operate ....", then continue running the next command.

```
sh initial_server_2.sh &
```

 Wait until it outputs "ready to operate ....", then continue running the next command.

```
sh initial_server_3.sh &
```

Wait until it outputs "ready to operate ....", then continue running the next command.

Now that all the servers are ready, you can run
```
sh run-different-line-numbers.sh  <topic> <fsa_address> <rhyme_address> <log_address> <related_word_add> <number_of_lines>
```

By executing this command, the program generates related words, rhymes, and an fsa in the given addresses. The topic can be any word or phrase. Right now, we just support <number_of_lines>=2,4,14.



Send any questions or comments to mghazvin@usc.edu
