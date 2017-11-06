TARGET = parse
LIBS = -lm
CC = gcc
CFLAGS = -g -Wall

.PHONY: default all clean

default: $(TARGET)
all: default
re: clean default all

OBJECTS = $(patsubst %.c, %.o, $(wildcard *.c))
HEADERS = $(wildcard *.h)

%.o: %.c $(HEADERS)
	@$(CC) $(CFLAGS) -c $< -o $@

$(TARGET): $(OBJECTS)
	@$(CC) $(OBJECTS) -Wall $(LIBS) -o $@

# Targets used to add debug flag
.debug-add-flag:
	@$(eval CFLAGS = $(CFLAGS) -D DEBUG)
debug: clean .debug-add-flag default all

# Targets used to add disable flag(s)
.disable-color:
	@$(eval CFLAGS = $(CFLAGS) -D MONOCHROME)
.disable-source-display:
	@$(eval CFLAGS = $(CFLAGS) -D DISABLE_SOURCE_DISPLAY)
.disable-tab_size-warning:
	@$(eval CFLAGS = $(CFLAGS) -D DISABLE_TAB_SIZE_WARNING)
	
# Targets used to prepare for test
TEST_DIR := test
TEST_OUTPUT_MATCHER_SCRIPT := $(TEST_DIR)/output_match.sh
TEST_TEMP_ERROR_OUTCOME := $(TEST_DIR)/temp_error_output
TEST_SOURCE_DIR := case
TEST_EXPECTED_OUTCOME_DIR := expected_outcome
TEST_SOURCE_FILES := $(notdir $(sort $(shell find ./$(TEST_DIR)/$(TEST_SOURCE_DIR) -regextype posix-extended -regex './$(TEST_DIR)/$(TEST_SOURCE_DIR)/[0-9]+\.txt')))

.test-run:
	@for file in $(TEST_SOURCE_FILES) ; do echo "Running test: $$file"; ./$(TARGET) ./$(TEST_DIR)/$(TEST_SOURCE_DIR)/$$file ; done

.test-check:
	@chmod +x $(TEST_OUTPUT_MATCHER_SCRIPT)
	@for file in $(TEST_SOURCE_FILES) ; do												\
		./$(TARGET) ./$(TEST_DIR)/$(TEST_SOURCE_DIR)/$$file > $(TEST_TEMP_ERROR_OUTCOME); 					\
		$(TEST_OUTPUT_MATCHER_SCRIPT) $$file $(TEST_TEMP_ERROR_OUTCOME) $(TEST_DIR)/$(TEST_EXPECTED_OUTCOME_DIR)/$$file;	\
		done
.test-clean:
	@-rm -f $(TEST_TEMP_ERROR_OUTCOME)
test: clean .disable-color .disable-source-display default all .test-check .test-clean
test-run: clean default all .test-run
test-no-warning: clean .disable-tab_size-warning default all .test-run
test-debug: clean debug all .test-run

clean:
	@-rm -f $(TEST_TEMP_ERROR_OUTCOME)
	@-rm -f *.o
	@-rm -f $(TARGET)