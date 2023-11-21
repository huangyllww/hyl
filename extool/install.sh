#! /bin/bash

CurrentBash=$(echo $SHELL)

if [[ "$CurrentBash" == '/bin/zsh' ]]
then
    if [[ -z "$(cat ~/.zshrc | grep  "source AutoSource.sh")" ]]
    then
        echo '' >> ~/.zshrc
        echo '#Add extool start self' >> ~/.zshrc
        echo 'tmp_path=$(pwd)' >> ~/.zshrc
        echo cd $(pwd) >> ~/.zshrc
        echo 'source AutoSource.sh' >> ~/.zshrc
        echo 'cd $tmp_path' >> ~/.zshrc

        echo "extool for zsh install sucess, please restart terminal"
    fi
elif [[ "$CurrentBash" == '/bin/bash' ]]
then 
    if [[ -z "$(cat ~/.bashrc | grep  "source AutoSource.sh")" ]]
    then
        echo '' >> ~/.bashrc
        echo '#Add extool start self' >> ~/.bashrc
        echo 'tmp_path=$(pwd)' >> ~/.bashrc
        echo cd $(pwd) >> ~/.bashrc
        echo 'source AutoSource.sh' >> ~/.bashrc
        echo 'cd $tmp_path' >> ~/.bashrc

        echo "extool for bash install sucess, please restart terminal"
    fi
else
    #默认放在 /etc/profile.d/bash_completion.sh
    if [[ -z "$(cat /etc/profile.d/bash_completion.sh | grep  "source AutoSource.sh")" ]]
    then
        echo '' >> /etc/profile.d/bash_completion.sh
        echo '#Add extool start self' >> /etc/profile.d/bash_completion.sh
        echo 'tmp_path=$(pwd)' >> /etc/profile.d/bash_completion.sh
        echo cd $(pwd) >> /etc/profile.d/bash_completion.sh
        echo 'source AutoSource.sh' >> /etc/profile.d/bash_completion.sh
        echo 'cd $tmp_path' >> /etc/profile.d/bash_completion.sh

        echo "extool for default install sucess, please restart terminal"
    fi
fi

