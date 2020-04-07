pipeline {
    agent { 
        label 'sibr'
    }
    post {
        failure {
            updateGitlabCommitStatus name: 'build', state: 'failed'
        }
        success {
            updateGitlabCommitStatus name: 'build', state: 'success'
        }
    }
    options {
        gitLabConnection('Gitlab Inria')
        gitlabBuilds(builds: ['Configure', 'Build', 'Test'])
    }
    triggers {
        gitlab(triggerOnPush: true, triggerOnMergeRequest: true, branchFilterType: 'All')
    }
    environment {
        CMAKE_PATH='cmake.exe'
        MSBUILD_PATH='C:\\Program Files (x86)\\Microsoft Visual Studio\\2019\\Community\\MSBuild\\Current\\Bin\\MSBuild.exe'
        CONFIGURATION='RelWithDebInfo'
    }
    stages {
        stage('Configure') {
            steps {
                bat '%CMAKE_PATH% . -B build -DBUILD_IBR_ULR=ON -DBUILD_DOCUMENTATION=ON'
            }
        }
        stage('Build') {
            steps {
                bat '%MSBUILD_PATH% build\\ALL_BUILD.vcxproj /p:Configuration=RelWithDebInfo'
                bat '%MSBUILD_PATH% build\\INSTALL.vcxproj /p:Configuration=RelWithDebInfo'
                bat '%MSBUILD_PATH% build\\docs\\doxygen\\compileDocs.vcxproj'
            }
        }
        stage('Test') {
            steps {
                bat 'echo no test defined...'
            }
        }
    }
}
