# Folder in which to create assembler (S) files for including images data. 
S_DIR = S
# Folder where source files are located.
JPG_DIR = .
# Relative path to JPG files.
#RELDIR = "../../../Images"
RELDIR = 

JPG_FILES := $(wildcard $(JPG_DIR)/*.jpg)
S_FILES := $(patsubst $(JPG_DIR)/%.jpg,$(S_DIR)/%.s,$(JPG_FILES))

#-------------------------------------------------------------------------------
# All targets
#-------------------------------------------------------------------------------
.PHONY: all
all: dirs $(S_FILES)

dirs: $(S_DIR)

$(S_DIR):
	@echo 'Creating directory: $@'
	-mkdir $(S_DIR)

# Each subdirectory must supply rules for building sources it contributes
$(S_DIR)/%.s: $(JPG_DIR)/%.jpg
	@echo 'Processing file: $<'
	@echo 'Making S file: $@'
	$(eval SYMNAME = $(subst .,_, $(notdir $<)))
	@echo 'Symbol: img_$(SYMNAME)'
	@echo '### Auto-generated file by images.mk ###' > $@
	@echo '.align 4' >> $@
	@echo '.global img_$(SYMNAME)' >> $@
	@echo 'img_$(SYMNAME):' >> $@
	@echo '.incbin "$(RELDIR)$<"' >> $@
	@echo 'Finished making file: $@'
	@echo ' '

.PHONY: clean
clean:
	-$(RM) $(S_FILES) 
	-@echo ' '
