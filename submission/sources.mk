# sources.mk
# ----------
# Edit this file!

# +-- Set to 0 for English board output --+
CHINESE = 1

SRC_DIR = algorithm
INC_DIR = algorithm


# include flags
INC_FLAGS = -I. -Ilib board_analyzer.h evaluator.h TT.h AB_agent.h

# gather cpp from all SRC_DIRs
MODULE_SRC = board_analyzer.cpp evaluator.cpp TT.cpp AB_agent.cpp

ADD_SOURCES = $(INC_FLAGS) $(MODULE_SRC)

# +-- Add your own sources here, if any --+
# ADD_SOURCES = -I. -Ilib -I$(INC_DIR) $(wildcard $(SRC_DIR)/*.cpp)
# ADD_SOURCES = 