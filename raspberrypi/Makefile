default: compile

upgrade-all: upgrade-chef upgrade-arduino upgrade-fpga upgrade-filesystem

# CONSTANTES

# Périphérique bloc à utiliser pour flasher
SDCARD=/dev/mmcblk0
CON_MODE=wifi
ETH_IFACE=enp3s0

# Debug
TARGET_DIR=$(PWD)/buildroot/output/target/
EXECDIR=/opt/chef/
EXECPATH=bin/premier
GDBPORT=2346

# SYSTÈME D'EXPLOITATION

# Configuration
buildroot/.config: configs/cdfprincipal_defconfig
	make -C buildroot BR2_EXTERNAL=.. cdfprincipal_defconfig

configure: buildroot/.config

# Compile l'OS (ça prend quelques heures)
compile: configure
	make -C buildroot target-finalize

# Crée l'image à flasher
image:
	make -C buildroot

# Supprime tous les fichiers de l'OS
clean:
	make -C buildroot clean

# Dernier recours en cas de compilation d'OS foireuse
# (faut être patient)
recompile: clean compile

# Flashe l'image sur la carte SD
flash: image $(SDCARD)
	sudo dd status=progress if=buildroot/output/images/sdcard.img of=$(SDCARD) bs=1M
	sync
	echo -e "resizepart 2\n100%\nquit" | sudo parted $(SDCARD)
	sync
	sudo resize2fs $(SDCARD)*2
	sync

# Graphiques (parce que c'est rigolo)
graphs:
	make -C buildroot graph-{size,build,depends{,-requirements}}

# CONNEXION AU ROBOT

# Il vous faudra pour ces actions le fichier principalconf.sh

# Crée un fichier de conf utilisable pour s'y connecter
sshconf: principalconf.sh
ifeq ($(CON_MODE), eth)
	source $$PWD/$<; echo -e "Host principal p\n    User root\n    Hostname $$ETHADDRESS\n    PreferredAuthentications publickey\n    PubkeyAuthentication yes\n    IdentityFile \"$$PWD/sshkey\"" > "$@"
	source $$PWD/$<; sudo ip address add dev $(ETH_IFACE) $${ETHPEER}/20 peer $${ETHADDRESS}
else
	source $$PWD/$<; echo -e "Host principal p\n    User root\n    Hostname $$ADDRESS\n    PreferredAuthentications publickey\n    PubkeyAuthentication yes\n    IdentityFile \"$$PWD/sshkey\"" > "$@"
endif
	source $$PWD/$<; echo -e "$$SSHCPRV" > sshkey
	chmod 600 sshkey

# Lance une connexion SSH, tout bêtement
ssh: sshconf
	ssh -F sshconf principal

# Redémarre le robot
reboot: sshconf
	ssh -F sshconf principal /sbin/reboot

# Met le robot à l'heure (jusqu'au prochain redémarrage)
# (ce qui est une mauvaise idée parce que du coup rsync
# n'écrase pas les fichiers modifiés directement sur le Pi)
#date: sshconf
#	ssh -F sshconf principal "date -s $(date +%Y%m%d%H%M.%S)"

# Met à jour le Raspberry Pi (par rapport à ce dépôt)
upgrade-filesystem: sshconf configure
	make -C buildroot target-finalize
	@# TODO Récupérer les ACL plutot que de mettre tous les fichiers en root
	ssh -F sshconf principal true
	rsync --rsh 'ssh -F sshconf' --archive --chown root:root buildroot/output/target/ principal:/

# Met jour les overlays (une partie des fichiers)
upgrade-overlays: sshconf
	ssh -F sshconf principal true
	chmod 700 board/robotech/cdfprincipal/rootfs_overlay/root
	rsync --rsh 'ssh -F sshconf' --archive --chown root:root board/robotech/cdfprincipal/rootfs_overlay/ principal:/

# ARDUINO
upgrade-arduino:
	make -C ../arduino/
	scp -F sshconf -q "../arduino/principal.hex" principal:/tmp/principal.hex
	ssh -F sshconf principal "avrdude -p atmega2560 -P /dev/ttyACM0 -c stk500 -D -U flash:w:/tmp/principal.hex"

# FPGA
upgrade-fpga:
	make -C ../fpga/
	scp -F sshconf -q "../fpga/build/Principal.bit" principal:/tmp/Principal.bit
	ssh -F sshconf principal "mercpcl /tmp/Principal.bit"

# CHEF

chef:
	make -C buildroot chef-rebuild

upgrade-chef: chef
	ssh -F sshconf principal true
	rsync --rsh 'ssh -F sshconf' --archive --chown root:root buildroot/output/target/opt/chef principal:/opt/

run:
	ssh -F sshconf principal true
	ssh -F sshconf principal /etc/init.d/S50chef stop
	ssh -F sshconf principal 'cd /opt/chef; bin/premier; /opt/chef/lcdOff.sh'

restart:
	ssh -F sshconf principal true
	ssh -F sshconf principal /etc/init.d/S50chef restart

getlogs:
	ssh -F sshconf principal true
	rsync --rsh 'ssh -F sshconf' --archive principal:/opt/chef/log/* ../log/

gdbcommands:
	echo "set sysroot $(TARGET_DIR)" > "$@"
	echo "exec-file $(TARGET_DIR)$(EXECDIR)$(EXECPATH)" >> "$@"
	echo "symbol-file $(TARGET_DIR)$(EXECDIR)$(EXECPATH).debug" >> "$@"
	echo "target remote :$(GDBPORT)" >> "$@"
	echo "continue" >> "$@"

debug: gdbcommands
	ssh -F sshconf principal true
	ssh -F sshconf principal /etc/init.d/S50chef stop
	ssh -F sshconf principal killall gdbserver; true
	ssh -F sshconf principal -L $(GDBPORT):127.0.0.1:$(GDBPORT) 'cd $(EXECDIR); gdbserver 127.0.0.1:$(GDBPORT) $(EXECPATH)' & sleep 1
	buildroot/output/host/usr/bin/arm-linux-gnueabihf-gdb -x gdbcommands; \
		ssh -F sshconf principal /opt/chef/lcdOff.sh; \
		ssh -F sshconf principal killall gdbserver; true

.PHONY: gdbcommands
