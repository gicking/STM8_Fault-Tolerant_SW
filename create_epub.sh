#!/bin/bash  

# open terminal on double-click, skip else. See https://www.linuxquestions.org/questions/programming-9/executing-shell-script-in-terminal-directly-with-a-double-click-370091/
tty -s; if [ $? -ne 0 ]; then /etc/alternatives/x-terminal-emulator -e "$0"; exit; fi

# change to current working directory
cd `dirname $0`

# just for output
echo off
clear

# convert Markdown to ePub using pandoc
pandoc README.md --metadata title="Fault-Tolerant Software for STM8 Microcontrollers" -o Fault-Tolerant_SW.epub

# rename PDF (exported from Visual Studio Code)
mv README.pdf Fault-Tolerant_SW.pdf
