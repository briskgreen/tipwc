# tipwc
c语言ini解析库

简介
====
    tipwc是一个轻量级的c语言ini解析库，使用它可以解析ini文件也可以创建ini文件。

使用方法
========
    tipwc所使用数据结构为INI，该结构中有两个参数一个指示表的数据另一个为表的指针，该表是一个链表，每一个表中的表名为ini文件中的一个“节点”（介于[与]之间的字符串），而该表中的node是一个指向该节点下所有参数与值的一个链表，其中包括键名与键值（ini文件中=字符两边的字符串）。  
    tipwc的使用很简单，解析时只需要使用ini_parse或者ini_parse_from_filename就可以获得一个解析后的INI结构，然后可以使用ini_get_key_value来取得指定节点下指定键的键值。创建ini文件时使用ini_table_add添加一个节点，使用ini_node_add可以向指定的节点中添加一个参数对，完成一切后可以使用ini_build生成ini配置文件并获得一个字符串结果，或者使用ini_build_to_file将ini配置写入到文件中。

所使用到的依赖
==============
    使用到的依赖为sbuf，sbuf是一个c语言“动态字符串”，sbuf的地址为 ： https://github.com/briskgreen/sbuf
