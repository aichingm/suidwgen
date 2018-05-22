#
#    SUIDWGEN - set uid wrapper generator
#    Copyright (C) 2018  Mario Aichinger <aichingm@gmail.com>
#
#    This program is free software: you can redistribute it and/or modify
#    it under the terms of the GNU General Public License as published by
#    the Free Software Foundation, either version 3 of the License, or
#    (at your option) any later version.
#
#    This program is distributed in the hope that it will be useful,
#    but WITHOUT ANY WARRANTY; without even the implied warranty of
#    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#    GNU General Public License for more details.
#
#    You should have received a copy of the GNU General Public License
#    along with this program.  If not, see <https://www.gnu.org/licenses/>.
#


SCRIPT:=/dev/null
INTERPRETER:=/bin/false
USER=root
CHECK_SCRIPT_NOT_WRITABLE:=1
CHECK_SCRIPT_NOT_READABLE:=1
CHECK_SCRIPT_ONLY_USR_EXEC:=1
CHECK_SAME_OWN:=1
CHECK_SAME_GRP:=1
CHECK_NOT_ROOT_U:=0
CHECK_NOT_ROOT_G:=0
MAX_ARGS:=-1
UNSET_ENV:=1
DEBUG:=0

CHECK_MAX_ARGS:=0
CHECK_HASH:=1

NEW_SCRIPT:=$(SCRIPT)-$(USER)
SCRIPT_WRAPPER:=$(SCRIPT)-$(USER)-wrapper
SCRIPT_WRAPPER_C:=$(SCRIPT_WRAPPER).c

clean:
	sudo rm $(SCRIPT_WRAPPER_C) || true
	sudo rm $(SCRIPT_WRAPPER) || true
	sudo rm $(NEW_SCRIPT) || true

wrapper: clean
	sudo cp -r $(SCRIPT) $(NEW_SCRIPT)
	touch $(SCRIPT_WRAPPER_C)
	echo -n >  $(SCRIPT_WRAPPER_C)
	echo "" >> $(SCRIPT_WRAPPER_C)
	echo "#define SCRIPT \""$(NEW_SCRIPT)"\"" >>  $(SCRIPT_WRAPPER_C)
	echo "#define INTERPRETER \""$(INTERPRETER)"\"" >>  $(SCRIPT_WRAPPER_C)
	if [[ $(CHECK_SCRIPT_NOT_WRITABLE) = 1 ]]; then echo "#define CHECK_SCRIPT_NOT_WRITABLE 1" >>  $(SCRIPT_WRAPPER_C); fi
	if [[ $(CHECK_SCRIPT_NOT_READABLE) = 1 ]]; then echo "#define CHECK_SCRIPT_NOT_READABLE 1" >>  $(SCRIPT_WRAPPER_C); fi
	if [[ $(CHECK_SCRIPT_ONLY_USR_EXEC) = 1 ]]; then echo "#define CHECK_SCRIPT_ONLY_USR_EXEC 1" >>  $(SCRIPT_WRAPPER_C); fi
	if [[ $(CHECK_SAME_OWN) = 1 ]]; then echo "#define CHECK_SAME_OWN 1" >>  $(SCRIPT_WRAPPER_C); fi
	if [[ $(CHECK_SAME_GRP) = 1 ]]; then echo "#define CHECK_SAME_GRP 1" >>  $(SCRIPT_WRAPPER_C); fi
	if [[ $(CHECK_NOT_ROOT_U) = 1 ]]; then echo "#define CHECK_NOT_ROOT_U 1" >>  $(SCRIPT_WRAPPER_C); fi
	if [[ $(CHECK_NOT_ROOT_G) = 1 ]]; then echo "#define CHECK_NOT_ROOT_G 1" >>  $(SCRIPT_WRAPPER_C); fi
	if [[ $(CHECK_HASH) = 1 ]]; then echo "#define CHECK_SCRIPT_SUM \""$$(sha1sum $(SCRIPT) | cut -d ' ' -f 1)"\"" >>  $(SCRIPT_WRAPPER_C); fi
	if [[ $(CHECK_MAX_ARGS) = 1 ]]; then echo "#define MAX_ARGS" $(MAX_ARGS) >>  $(SCRIPT_WRAPPER_C); fi
	if [[ $(UNSET_ENV) = 1 ]]; then echo "#define UNSET_ENV 1" >>  $(SCRIPT_WRAPPER_C); fi
	if [[ $(DEBUG) = 1 ]]; then echo "#define DEBUG 1" >>  $(SCRIPT_WRAPPER_C); fi
	if [[ 1 = 1 ]]; then echo "#define ENABLE_HELP 1" >>  $(SCRIPT_WRAPPER_C); fi
	cat suidwgen.c >> $(SCRIPT_WRAPPER_C)
	if [[ $(CHECK_HASH) = 1 ]]; then LIB="-lssl -lcrypto" ; else LIB="";fi; gcc $$(echo $$LIB) -o $(SCRIPT_WRAPPER) $(SCRIPT_WRAPPER_C)

secure:
	sudo chmod 700 $(NEW_SCRIPT); 
	sudo chmod 711 $(SCRIPT_WRAPPER); 
	sudo chown $(USER):$(USER) $(NEW_SCRIPT); 
	sudo chown $(USER):$(USER) $(SCRIPT_WRAPPER); 
	sudo chmod a+s $(SCRIPT_WRAPPER); 

all: clean wrapper secure
