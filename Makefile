LIB_NAME		= libtorrent_core.a
TERMINAL_NAME	= torrent_client
QT_NAME			= Torrent\ Client\ Qt

# build libtorrent_core.a
SRCS_PATH		= ./srcs
SRCS_FILES		= TorrentClient.cpp \
					TorrentFileParser.cpp \
					PeerRetriever.cpp \
					PeerConnection.cpp \
					PieceManager.cpp \
					Piece.cpp \
					Message.cpp \
					PeersQueue.cpp \
					utils.cpp \
					connection.cpp

OBJS_PATH	= ./objs
OBJS_FILES	= ${SRCS_FILES:.cpp=.o}
OBJS		= ${addprefix ${OBJS_PATH}/, ${OBJS_FILES}}
DEPS_LIB	= ${OBJS:%.o=%.d}

# build terminal-based program
TERMINAL_SRCS_PATH	= ./terminal_main
TERMINAL_SRCS_FILES	= main.cpp
TERMINAL_OBJS_PATH	= ./terminal_objs
TERMINAL_OBJS_FILES	= ${TERMINAL_SRCS_FILES:.cpp=.o}
TERMINAL_OBJS		= ${addprefix ${TERMINAL_OBJS_PATH}/, ${TERMINAL_OBJS_FILES}}
DEPS_TERMINAL		= ${TERMINAL_OBJS:%.o=%.d}

# build GUI
QT_DIR		= ./Qt
QT_SRCS		= main_Qt.cpp Window.cpp Window.hpp
QT_FILES	= ${addprefix ${QT_DIR}/, ${QT_SRCS}}

INC_DIR		= ./incs
INC			= ${INC_DIR} ./bencode ./spdlog ./cxxopts
INC_QT		= ${INC:%=../%}
INCLUDE		= ${INC:%=-I %}

LDLIBS		= -lcryptopp -lcurl

CXX			= g++
CXXFLAGS	= -std=c++17 -Wall -Wextra -O2 -march=native
#-g -O0 -fsanitize=address -fsanitize=leak -fsanitize=undefined #-fsanitize=thread
#-g3 -O0 -fsanitize=thread

LIBC		= ar rcs

RM			= rm -rf

all: ${LIB_NAME} ${TERMINAL_NAME} ${QT_NAME}

${LIB_NAME}: ${OBJS}
	${LIBC} ${LIB_NAME} ${OBJS}

${TERMINAL_NAME}: ${LIB_NAME} ${TERMINAL_OBJS}
	${CXX} ${CXXFLAGS} ${INCLUDE} ${TERMINAL_OBJS} ${LIB_NAME} ${LDLIBS} -o ${TERMINAL_NAME}

${QT_NAME}: ${LIB_NAME} ${QT_FILES}
	cd ${QT_DIR};\
	qmake -project -o ${QT_NAME} \
		"QT=widgets" \
		"INCPATH+=${INC_QT}" \
		"LIBS+=../${LIB_NAME} ${LDLIBS}" \
		"QMAKE_CXXFLAGS+=${CXXFLAGS}";\
	qmake;\
	make -C .;\
	mv ${QT_NAME} ../


# lib files
${OBJS_PATH}/%.o : ${SRCS_PATH}/%.cpp | ${OBJS_PATH}
	${CXX} ${CXXFLAGS} ${INCLUDE} -MMD -MP -c $< -o $@

-include ${DEPS_LIB}

${OBJS_PATH}:
	mkdir -p ${OBJS_PATH}


# terminal-based files
${TERMINAL_OBJS_PATH}/%.o : ${TERMINAL_SRCS_PATH}/%.cpp | ${TERMINAL_OBJS_PATH}
	${CXX} ${CXXFLAGS} ${INCLUDE} -MMD -MP -c $< -o $@

-include ${DEPS_TERMINAL}

${TERMINAL_OBJS_PATH}:
	mkdir -p ${TERMINAL_OBJS_PATH}


clean:
	${RM} ${OBJS_PATH} ${TERMINAL_OBJS_PATH}
	make clean -C ${QT_DIR} 2> /dev/null; true

fclean: clean
	${RM} ${LIB_NAME} ${TERMINAL_NAME} ${QT_NAME}
	${RM} ${QT_DIR}/.qmake.stash ${QT_DIR}/Makefile ${QT_DIR}/${QT_NAME}.pro

re: fclean all

format:
	clang-format -i ${SRCS_PATH}/* ${INC_DIR}/* \
		${TERMINAL_SRCS_PATH}/* \
		${QT_FILES}

.PHONY: all clean fclean re format