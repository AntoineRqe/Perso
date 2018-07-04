apt-get update
apt-get upgrade
apt-get install terminator geany geany-plugins git gitk ssh vim
git config --global alias.co checkout
git config --global alias.ci commit
git config --global alias.br branch
git config --global alias.fe fetch -a
git config --global alias.st status
git config --global alias.last 'log -1 HEAD'
git config --global alias.unstage 'reset HEAD --'
git config --global user.name "Antoine Rouquette"
git config --global user.email "antoine.rqe@gmail.com"

echo "antoine ALL=(ALL) NOPASSWD: ALL" >> /etc/sudoers
