This is a simple tool I write to list all the files existed in a specific file system in order to get to know about more information in a disk and track the timeline of some files we may concern. Notice that This is a project for learning. The implementation refers to that wonderful tool TSK(The Sleuth Kit)(www.sleuthkit.org) and the book FSFA(File System Forensic Analysis)(http://www.digital-evidence.org/fsfa/index.html) written by Brain Carrier.

The main page of this program is http://helenawang.github.io/FSAnalyzer. There you will find more information about it.

Now there are two versions of implementation:
version 1: in folder "old ver-read ascii"
	This is implemented by using "xxd" command then read ascii, which was as what I originally did; 
version 2: in folder "new ver-read file"
	This is to read binary files directorly using file access function(fread,fseek...) in C, which I tried another approach in order to reduce the machine-dependence.
You may download either or both of them then make and run them respectively. Detailed make instructions please refer to readme in each folder.
