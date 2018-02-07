#!/bin/sh

if file ${BR2_EXTERNAL_CDF_PATH}/principalconf.sh &> /dev/null
then
    source ${BR2_EXTERNAL_CDF_PATH}/principalconf.sh

    # Wi-Fi configuration
    mkdir -p ${TARGET_DIR}/etc/wpa_supplicant/
    wpa_passphrase "$WPASSID" "$WPAPSK" > ${TARGET_DIR}/etc/wpa_supplicant/wpa_supplicant.conf
    # Le ifup de Busybox ne supporte pas les options wpa-*, donc on utilisera les wpa_supplicant en direct

    # Network configuration
    echo -e "
auto wlan0
iface wlan0 inet static
  address $ADDRESS
  netmask $NETMASK
  gateway $GATEWAY
" >> ${TARGET_DIR}/etc/network/interfaces

    # SSH configuration
    rm ${TARGET_DIR}/etc/dropbear &> /dev/null
    mkdir -p ${TARGET_DIR}/etc/dropbear/
    echo "$SSHSPRV" | base64 -d > ${TARGET_DIR}/etc/dropbear/dropbear_ecdsa_host_key
    mkdir -p ${TARGET_DIR}/root/.ssh/
    echo "$SSHCPUB" > ${TARGET_DIR}/root/.ssh/authorized_keys
else
    echo "Récupérez le fichier principalconf.sh pour pouvoir vous connecter au vrai robot !"
fi

# Demote some services
for service in S20urandom S40network S50dropbear
do
    if [ -f ${TARGET_DIR}/etc/init.d/${service} ]
    then
        mv ${TARGET_DIR}/etc/init.d/${service} ${TARGET_DIR}/etc/extra.d/${service}
    fi

done

dd if=/dev/urandom of=${TARGET_DIR}/etc/random-seed bs=512 count=1

