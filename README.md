# Blather Chat Room

## 服务端

对整个chat room进行管理， 只输出debug信息。

## 客户端

用户在客户端中输入文字，然后发送给服务端，服务端将这段文字转发给其他所有客户端。

## Note

blather被限制只在单机运行，通过管道通信，只有能读写相关文件的用户可以通过其进行交流。但拓展后也可以支持网络通信。

| File              | State    | Notes                                                                |
| ----------------- | -------- | -------------------------------------------------------------------- |
| GROUP_MEMBERS.txt | Edit     | Fill in names of group members to indicate partnerships              |
| Makefile           | Create   | Build project, run tests                                             |
| server_funcs.c    | Create   | Service routines for the server_actual                                      |
| bl_server.c       | Create   | Main function for bl_server executable                               |
| bl_client.c       | Create   | Main function for bl_client executable                               |
| blather.h         | Provided | Header file which contains required structs, defines, and prototypes   |
| util.c            | Provided | Utility methods debug messages and checking system call returns      |
| simpio.c          | Provided | Simplified terminal I/O to get nice interactive sessions              |
| simpio_demo.c     | Provided | Demonstrates simpio features, model for bl_client.c                  |
| Testing           | TBD      | Will be released separately.                                         |