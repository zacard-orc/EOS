EOS
===

A Crazy Streaming Compute Engine [C++]

Opening Soon.........

Progress  
===  
2014.04.30 5%  epoll server+mq  
2014.05.02 15% map server  
2014.05.03 25% reduce server    
2014.05.04 30% all road firstly  
2014.05.06 35% add auto-reconnect & connect timeout  
2014.05.09 40% add backgroud runtime  
2014.05.17 45% add check network status  
2014.05.18 47% add duplex list for the base of memory pool  
2014.05.20 48% fix the network status checking  


Usage    
===    
1, make     
2, ./reduce_srv|./fun_redu   
3, ./epoll_srv    
4, ./map_msg|./fun_map|./tcp_cli  

fun_map and fun_redu is your custom map-reduce code based on stdin & stdout.Just like hadoop streaming.  
