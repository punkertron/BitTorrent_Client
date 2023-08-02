NAME		= torrent_client

SRCS_PATH	= ./srcs
SRCS_FILES	= main.cpp \
				TorrentFileParser.cpp \
				utils.cpp

OBJS_PATH	= ./objs
OBJS_FILES	= ${SRCS_FILES:.cpp=.o}
OBJS		= ${addprefix ${OBJS_PATH}/, ${OBJS_FILES}}

INC_DIR		= ./incs
INC			= ${INC_DIR} ./bencode
INCLUDE		= $(INC:%=-I %)

LDLIBS		= -lcryptopp

DEPS		= ${OBJS:%.o=%.d}

CXX			= g++
CXXFLAGS	= -std=c++17 #-g -fsanitize=address -fsanitize=leak -fsanitize=undefined

RM			= rm -rf

all: ${NAME}

${NAME}: ${OBJS}
	${CXX} ${CXXFLAGS} ${INCLUDE} ${OBJS} ${LDLIBS} -o ${NAME}

${OBJS_PATH}/%.o : ${SRCS_PATH}/%.cpp | ${OBJS_PATH}
	${CXX} ${CXXFLAGS} ${INCLUDE} -MMD -MP -c $< -o $@

${OBJS_PATH}:
	mkdir -p ${OBJS_PATH}

-include ${DEPS}

clean:
	${RM} ${OBJS_PATH}

fclean: clean
	${RM} ${NAME}

re: fclean | ${OBJS_PATH} ${NAME}

format:
	clang-format -i ${SRCS_PATH}/* ${INC_DIR}/*

.PHONY: all clean fclean re format