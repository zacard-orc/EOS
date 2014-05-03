CXX:=g++
LIB_FLAGS=-ltools -lpthread
STD:=-std=c++0x
LIB_DIR:=./lib
SRC_DIR:=./src2
INCLUDES_DIR:=./include
AR:=ar
ARFLAG:=cqs
BAN1:=@echo "Author Zacard: ";
BAN2:=@echo "--------------------------";


all:ann genlib arlib genobj genprog clean 
ann:
	$(BAN1)
	$(BAN2)
genlib:
	$(CXX) -c -Wall $(SRC_DIR)/tools.cpp -I$(INCLUDES_DIR) -g -o  $(LIB_DIR)/l_tools.o
arlib:
	$(AR) $(ARFLAG) $(LIB_DIR)/libtools.a $(LIB_DIR)/l_tools.o
genobj:
	$(CXX) $(SRC_DIR)/epoll_srv.cpp -c -g -Wall -I$(INCLUDES_DIR) -L$(LIB_DIR) $(LIB_FLAGS) -o $(LIB_DIR)/epoll_srv.o;
	$(CXX) $(SRC_DIR)/map_msg.cpp -c -g -Wall -I$(INCLUDES_DIR) -L$(LIB_DIR) $(LIB_FLAGS) -o $(LIB_DIR)/map_msg.o;
	$(CXX) $(SRC_DIR)/del_msg.cpp -c -g -Wall -I$(INCLUDES_DIR) -L$(LIB_DIR) $(LIB_FLAGS) -o $(LIB_DIR)/del_msg.o;
	$(CXX) $(SRC_DIR)/reduce_srv.cpp -c -g -Wall -I$(INCLUDES_DIR) -L$(LIB_DIR) $(LIB_FLAGS) -o $(LIB_DIR)/reduce_srv.o;
genprog:
	$(CXX) $(LIB_DIR)/epoll_srv.o -L $(LIB_DIR) $(LIB_FLAGS)  -o epoll_srv
	$(CXX) $(LIB_DIR)/map_msg.o -L $(LIB_DIR) $(LIB_FLAGS)  -o map_msg
	$(CXX) $(LIB_DIR)/del_msg.o -L $(LIB_DIR) $(LIB_FLAGS)  -o del_msg
	$(CXX) $(LIB_DIR)/reduce_srv.o -L $(LIB_DIR) $(LIB_FLAGS)  -o reduce_srv
clean:cleanobj
cleanobj:
	rm -f $(SRC_DIR)/*.o
	rm -f $(LIB_DIR)/*.o
	rm -f *.o
