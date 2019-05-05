Notes - tool for creation and searching text records



    DESCRIPTION


The application uses external text editor (vi by default) for text editing

A note (or record) consists of actual text, a set of attached tags and two dates: creation 
and modification 


Records can be found by:
  - partial text
  - single or multiple tags
  - date range (creation and/or modification)
  - compound request

User data can be encrypted if required



    USAGE


ADD

  > add -tag t1 t2
  [user inputs text in vi editor]


SEARCH

- By text
  > find -txt world

- By tags
  - Find records with at least one specified tags attached
    > find -tag t1 t2 t3
  - Find records with all specified tags attached
    > find -tags t1 t2

- By date (YYYY-MM-DD) 
  - Creation
    - > find -after 2013-4-22 -before 2017-12-15
  - Modification
    - > find -mafter 2013-4-22 -mbefore 2017-12-15


EXAMPLE OF SEARCH RESULT

-----------------------------------------------------------------------------------------------------
|2(2)                                                                                               |
|                                                                                                   |
|t1 | t2 | t3 |                                                                                     |
|                                                                                                   |
|Example of a record                                                                                |
|                                                                                                   |                        
|You can see the number of records found and index of current record                                |
|on top left corner                                                                                 |
|                                                                                                   |
|Tags attached to the record are displayed lower                                                    |
|                                                                                                   |
|Record creation and modification dates are below                                                   |
|                                                                                                   |
|                                                                                                   |
|2017-Dec-03 / 2017-Dec-03                                                                          |
|[n - Next] [p - Prev] [e - Edit] [at - Add Tag] [dt - Del Tag] [delete - Del Rec] [q - Quit Search]|
-----------------------------------------------------------------------------------------------------


WORKING WITH SEARCH RESULTS

  - "n"      go to the next record
  - "p"      go to the previous record
  - "delete" mark record as deleted (will not be shown in search results)
  - "e"      edit record
  - "at"     attach a new tag to the record
  - "dt"     delete tag from the record
  - "q"      exit from search results



    ENCRYPTION


- By default, the application starts in encrypted mode and prompts user password.
  This password will be used for data encryption and decryption

- Command line option "-no-encryption" disables data encryption