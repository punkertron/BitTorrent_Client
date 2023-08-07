NAME			= torrent_client

DOWNLOADS_PATH	= ./downloads

SRCS_PATH		= ./srcs
SRCS_FILES		= main.cpp \
					TorrentClient.cpp \
					TorrentFileParser.cpp \
					PeerRetriever.cpp \
					PeerConnection.cpp \
					PieceManager.cpp \
					Piece.cpp \
					Message.cpp \
					utils.cpp \
					connection.cpp

OBJS_PATH	= ./objs
OBJS_FILES	= ${SRCS_FILES:.cpp=.o}
OBJS		= ${addprefix ${OBJS_PATH}/, ${OBJS_FILES}}

INC_DIR		= ./incs
INC			= ${INC_DIR} /usr/local/include ./bencode
INCLUDE		= $(INC:%=-I %)

LDLIBS		= -lcryptopp -lcurl

DEPS		= ${OBJS:%.o=%.d}

CXX			= g++
CXXFLAGS	= -std=c++17 -O2 #-g -O0 -fsanitize=address -fsanitize=leak -fsanitize=undefined

RM			= rm -rf

all: ${NAME}

${NAME}: ${OBJS}
	${CXX} ${CXXFLAGS} ${INCLUDE} ${OBJS} ${LDLIBS} -o ${NAME}

${OBJS_PATH}/%.o : ${SRCS_PATH}/%.cpp | ${OBJS_PATH} ${DOWNLOADS_PATH}
	${CXX} ${CXXFLAGS} ${INCLUDE} -MMD -MP -c $< -o $@

${OBJS_PATH}:
	mkdir -p ${OBJS_PATH}

${DOWNLOADS_PATH}:
	mkdir -p ${DOWNLOADS_PATH}

-include ${DEPS}

clean:
	${RM} ${OBJS_PATH}

fclean: clean
	${RM} ${NAME}

clean-downloads:
	${RM} ${DOWNLOADS_PATH}

re: fclean | ${OBJS_PATH} ${DOWNLOADS_PATH} ${NAME}

format:
	clang-format -i ${SRCS_PATH}/* ${INC_DIR}/*

.PHONY: all clean fclean  clean-downloads re format