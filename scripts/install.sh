#! /bin/bash
sudo chown root:root runTomahawk startTomahawk tomahawk

sudo cp runTomahawk /usr/local/bin/
sudo cp startTomahawk /usr/local/bin/
sudo cp tomahawk /etc/init.d/ 

sudo update-rc.d tomahawk defaults
