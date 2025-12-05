# sources.mk
# ----------
# Edit this file!

# +-- Set to 0 for English board output --+
CHINESE = 1

SRC_DIR = MCTS/CPP AlphaBetaSolver/CPP BoardAnalyze/CPP EvaluationFunction/CPP
INC_DIR = MCTS/H AlphaBetaSolver/H BoardAnalyze/H EvaluationFunction/H


# include flags
INC_FLAGS = -I. -Ilib $(addprefix -I,$(INC_DIR))

# gather cpp from all SRC_DIRs
MODULE_SRC = $(foreach d,$(SRC_DIR),$(wildcard $(d)/*.cpp))

ADD_SOURCES = $(INC_FLAGS) $(MODULE_SRC)

# +-- Add your own sources here, if any --+
# ADD_SOURCES = -I. -Ilib -I$(INC_DIR) $(wildcard $(SRC_DIR)/*.cpp)
# ADD_SOURCES = 