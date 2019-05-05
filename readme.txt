Notes - the application which allows to make simple text records that can be easily found later.



 - DESCRIPTION -


A note (or record) consists of actual text (body), set of attached tags and two dates: creation 
and modification. 

Each of mentioned parts can be used to locate required record. In other words
a record can be found by:
- Some part of text
- Single or multiple tags
- Date range
- And by mixed request

The application supports 3 ways to add a new data:
- Inline, used for short one line note
- Compose note in an external editor (implicitly calls vi text editor)
- Import data from a file

Other capabilities:
- Records can be edited (implicitly in vi editor)
- Tags can be modified 
- User data can be encrypted if required (using crypto++ library)
- Current user password can be changed on demand



 - EXAMPLES OF USAGE -


ADD

- Inline adding
  The following command will create a record with text "hello world!" and tags "t1", "t2"
  > add hello world! -tag t1 t2

- Adding in external editor
  > ae -tag t1 t2
  [here user types data in vi and record is saved after exiting from editor (:wq)]

- Import from file
  > import /home/user/file.txt -tag t1 t2


SEARCH

- By text
  > find -ftxt world

- By tags
  - Find all records which have at least one of provided tags
    > find -tag t1 t2 t3
  - Find all records which have all provided tags
    > find -tags t1 t2

- By date (note the format used for date YYYY-MM-DD) 
  - Creation
    - > find -after 2013-4-22 -before 2017-12-15
  - Modification
    - > find -mafter 2013-4-22 -mbefore 2017-12-15


EXAMPLE OF SEARCH RESULT

--------------------------------------------------------------------------
|2(2)                                                                    |
|                                                                        |
|t1 | t2 | t3 |                                                          |
|                                                                        |
|Example of multi-line record added using external text editor vi        |
|                                                                        |                        
|You can see the number of records found and index of current record     |
|                                                                        |
|Tags attached to the record are on top as well                          |
|                                                                        |
|Also record's creation and modification dates are below                 |
|                                                                        |
|                                                                        |
|2017-Dec-03 / 2017-Dec-03                                               |
--------------------------------------------------------------------------


WORKING WITH SEARCH RESULTS

- If user specifies -printall option then all found records are simply printed out sequentially
- Without this option the application prints one record at a time and user can do the following:
  - Type "n" to go to the next record
  - Type "p" to go to the previous record
  - Type "delete" to mark record as deleted (will not be shown in search results by default)
  - Type "e" to edit record (in vi)
  - Type "at" to attach a new tag to the record
  - Type "dt" to delete tag from the record
  - Type "q" to exit from search results


ENCRYPTION

- By default, the application starts in encrypted mode and will ask user for a password.
This password will be used for data encryption and decryption

- User can specify application command line argument "-no-encryption" to store plain non-encrypted 
data. Note that this argument should be used for each program execution

- To change current password, type command "change-password <here should go a new password>"


JUST CAT THE SOURCES TO GET COMPREHENSIVE HELP



 - COMPILING -


- The application can be compiled for unix and cygwin environments
- Define C++11 or higher standard for compiler
- Configure linking with crypto++, libboost_date_time, libboost_serialization
- Runtime environment should have vi text editor in $PATH
- Use -DCYGWIN to compile for cygwin
- Approximate steps
  - Install boost
  - Install libcrypto++ library (libcrypto++-dev libcrypto++-doc libcrypto++-utils)
  - Execute something like this: 
      g++ -std=c++11 *.cpp -I ~/boost/include/ ~/boost/lib/libboost_date_time.a ~/boost/lib/libboost_serialization.a -lcrypto++
- Folder with source code contains file "a.out" which is compiled application for x86 linux (built on Ubuntu 14.04 32bit, gcc 4.8.5)
