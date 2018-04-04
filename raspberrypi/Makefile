default: compile

upgrade-all: upgrade-chef upgrade-arduino upgrade-fpga upgrade-filesystem

# CONSTANTES

# Périphérique bloc à utiliser pour flasher
SDCARD=/dev/mmcblk0

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
	source $$PWD/$<; echo -e "Host principal p\n    User root\n    Hostname $$ADDRESS\n    PreferredAuthentications publickey\n    PubkeyAuthentication yes\n    IdentityFile \"$$PWD/sshkey\"" > "$@"
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
	rsync --rsh 'ssh -F sshconf' --archive --chown root:root buildroot/output/target/ principal:/

# Met jour les overlays (une partie des fichiers)
upgrade-overlays: sshconf
	rsync --rsh 'ssh -F sshconf' --archive --chown root:root robotech/chef/rootfs_overlay/ principal:/

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
	make -C buildroot chef-reinstall
	rsync --rsh 'ssh -F sshconf' --archive --chown root:root buildroot/output/target/opt/chef principal:/opt/

