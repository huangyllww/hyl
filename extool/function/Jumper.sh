function jumperKS()
{
    
    expect -c "
        set timeout 30;
        spawn ssh simsadmin@10.8.102.95 -p 51730;
        expect {
            "yes/no" { send \"yes\r\"; exp_continue }
            "password" { send \"sims@Admin#xyz\r\" }  
        }
        interact
    "
}

function jumperTJ()
{
    
    expect -c "
        set timeout 30;
        spawn ssh root@10.2.68.128 -p 51730;
        expect {
            "yes/no" { send \"yes\r\"; exp_continue }
            "password" { send \"sims@Admin#Root\r\" }  
        }
        interact
    "
}