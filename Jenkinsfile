pipeline{
    agent {docker 'node:latest'}
    stages{
        stage('prepare'){ // 准备构建环境
            steps{
                echo "准备构建环境..."
                echo "当前目录"
                sh 'pwd'

                echo "安装sshpass"
                echo "下载sshpass"
                sh 'wget https://sourceforge.net/projects/sshpass/files/sshpass/1.06/sshpass-1.06.tar.gz/download -O sshpass.tar.gz'
                echo "解压"
                sh 'tar -xvf sshpass.tar.gz'

                // echo "进入解压目录配置"
                dir('sshpass-1.06'){
                    sh 'pwd'
                    sh 'ls'
                    sh ' ./configure'
                    echo "执行sshpass安装命令"
                    sh 'make install'
                }

                echo "全局安装gitbook..."
                sh 'npm install gitbook-cli -g'
                sh 'gitbook -V'
                echo "clone 过程代码..."
                sh 'git clone ${GIT_REPO} gitbook'
            }
        }
        stage('build'){ // 构建
            steps{
                echo "进入代码目录"
                dir('gitbook'){
                    sh 'pwd'
                    echo "安装gitbook插件"
                    sh 'gitbook install ./'
                    echo "gitbook构建静态网页"
                    sh 'gitbook build'
                }
            }
        }
        stage('deploy'){ // 部署
            steps{
                echo "将_book文件夹的内容拷贝到服务器上"
                echo "进入代码目录"
                dir('gitbook'){
                    sh 'pwd'
                    echo "scp -o StrictHostKeyChecking=no可以不用输yes了"
                    sh 'sshpass -p ${SERVER_PASSWORD} scp -o StrictHostKeyChecking=no -r ./_book/* ${SERVER}:${OUTPUT_DESTINATION}'
                    echo "远程拷贝完成"
                }
                sh 'pwd'
                echo "项目部署完毕"
            }
        }
    }
}