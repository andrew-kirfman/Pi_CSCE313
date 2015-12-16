#!/bin/bash

# Andrew Kirfman & Margaret Baxter
# Machine Problem #3 Part #2
# Due Date: 10/22/15

# Read in the value of the pid that the user would like to examine.  
printf "Enter the pid of a process: " 
read PID

# If the value entered is not an integer, try again.  
while ! [ "$PID" -eq "$PID" ] 2>/dev/null
do
    printf "Number was not a valid integer.  Try again: " 
    read PID
done

# Search the system to make sure that the process exists.  
OLD_DIR=$(pwd)
cd /proc
ls | grep -w $PID > /dev/null
if [ $? == 1 ]; then
    echo "A Process with the given ID does not exist on this system! - Exiting!"
    exit 0
else
    echo "Process found on the system!" 
    
    cd ./$PID
    printf "\nProcess Information: " 
    
    printf "\n1) Identifiers\n"
    printf "   PID:   $(perl -l -0777 -ne '@f = /\(.*\)|\S+/g; print $f[0]' "/proc/$PID/stat") \n"
    printf "   PPID:  $(perl -l -0777 -ne '@f = /\(.*\)|\S+/g; print $f[3]' "/proc/$PID/stat") \n"
    printf "   EUID:  $(grep "Uid" /proc/$PID/status | awk '{print $3}') \n"
    printf "   EGID:  $(grep "Gid" /proc/$PID/status | awk '{print $3}') \n"
    printf "   RUID:  $(grep "Uid" /proc/$PID/status | awk '{print $2}') \n"
    printf "   RGID:  $(grep "Gid" /proc/$PID/status | awk '{print $2}') \n"
    printf "   FSUID: $(grep "Uid" /proc/$PID/status | awk '{print $5}') \n"
    printf "   FSGID: $(grep "Gid" /proc/$PID/status | awk '{print $5}') \n"
    
    printf "\n2) State: \n"
    printf "   State: $(cat "/proc/$PID/status" | grep "State" | sed 's/State:\t//')\n"
    
    printf "\n3) Thread Information: \n"
    printf "   Thread Count: $(perl -l -0777 -ne '@f = /\(.*\)|\S+/g; print $f[19]' "/proc/$PID/stat") \n"
    
    printf "\n4) Priority: \n"
    printf "   Priority Number: $(perl -l -0777 -ne '@f = /\(.*\)|\S+/g; print $f[17]' "/proc/$PID/stat") \n"
    printf "   Niceness Value:  $(perl -l -0777 -ne '@f = /\(.*\)|\S+/g; print $f[18]' "/proc/$PID/stat") \n"
    
    printf "\n5) Time Information: \n"
    printf "   stime:  $(perl -l -0777 -ne '@f = /\(.*\)|\S+/g; print $f[14]' "/proc/$PID/stat") \n"
    printf "   utime:  $(perl -l -0777 -ne '@f = /\(.*\)|\S+/g; print $f[13]' "/proc/$PID/stat") \n"
    printf "   cstime: $(perl -l -0777 -ne '@f = /\(.*\)|\S+/g; print $f[16]' "/proc/$PID/stat") \n"
    printf "   cutime: $(perl -l -0777 -ne '@f = /\(.*\)|\S+/g; print $f[15]' "/proc/$PID/stat") \n"
    
    printf "\n6) Address Space: \n"
    printf "   Startcode: $(perl -l -0777 -ne '@f = /\(.*\)|\S+/g; print $f[25]' "/proc/$PID/stat") \n"
    printf "   Endcode:   $(perl -l -0777 -ne '@f = /\(.*\)|\S+/g; print $f[26]' "/proc/$PID/stat") \n"
    printf "   ESP:       $(perl -l -0777 -ne '@f = /\(.*\)|\S+/g; print $f[28]' "/proc/$PID/stat") \n"
    printf "   EIP:       $(perl -l -0777 -ne '@f = /\(.*\)|\S+/g; print $f[29]' "/proc/$PID/stat") \n"
    
    printf "\n7) Resourses: \n"
    printf "   File Handles: $(grep "sockets:" "/proc/$PID/net/sockstat" | sed 's/sockets://') \n"
    printf "   Voluntary Context Switches: $(grep "voluntary" "/proc/$PID/status" | grep -v "non" | sed 's/voluntary_ctxt_switches:\t//')\n"
    printf "   Involuntary Context Switches: $(grep "nonvoluntary" "/proc/$PID/status" | sed 's/nonvoluntary_ctxt_switches:\t//')\n"
    
    printf "\n8) Processors: \n"
    printf "   Last processor: $(perl -l -0777 -ne '@f = /\(.*\)|\S+/g; print $f[38]' "/proc/$PID/stat") \n"
    printf "   Allowed Cores:  $(grep "Cpus_allowed_list" "/proc/$PID/status" | sed 's/Cpus_allowed_list:\t//') \n"
        
    printf "\n9) Memory Map: \n"
    
    rm -rf $OLD_DIR/memory_map.txt > /dev/null
    touch  $OLD_DIR/memory_map.txt
    printf "   Would you like to see the process's memory map (yes/no)? "
    read answer
    print=$false
    [ $answer == "yes" ] && echo ""; print=$true
    [ $answer != "yes" ] && echo "   Memory map will be stored in file memory_map.txt."
    
    
    echo "     Address      | Permissions | Offset | Device | Inode | Pathname | Size" >> $OLD_DIR/memory_map.txt
    [ $print -eq $true ] && echo "     Address      | Permissions | Offset | Device | Inode | Pathname | Size"
    
    NUMBER_OCCURRENCES=1
    cat /proc/$PID/smaps | grep -v '^[A-Z[:punct:]]' | while read line
    do
        MEMORY_SIZE=$(cat /proc/$PID/smaps | grep -w "Size:" | grep -A 1 -m $NUMBER_OCCURRENCES 'Size:' | tail -n 1 | sed 's/Size:\s*//')
        
        array=($line)
        FINAL_LINE=""
        for item in ${array[@]}
        do 
            FINAL_LINE="$FINAL_LINE$item | " 
        done
        FINAL_LINE="$FINAL_LINE$MEMORY_SIZE" 
        echo $FINAL_LINE >> $OLD_DIR/memory_map.txt
        [ $print -eq $true ] && echo $FINAL_LINE
        
        let NUMBER_OCCURRENCES=NUMBER_OCCURRENCES+1
    done
    
fi
    
# Clean up at the end
cd $OLD_DIR
printf "\nDone!\n"
