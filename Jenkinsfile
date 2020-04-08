pipeline {
    agent { 
        label 'sibr'
    }
    options {
        gitLabConnection('Gitlab Inria')
        gitlabBuilds(builds: ['1-configure', '2-build', '3-test', '4-deploy'])
    }
    triggers {
    gitlab(
      triggerOnPush: true,
      triggerOnMergeRequest: true,
      triggerOpenMergeRequestOnPush: "source",
      triggerOnNoteRequest: true,
      noteRegex: "JenkinsRebuild",
      skipWorkInProgressMergeRequest: true,
      ciSkip: true,
      setBuildDescription: true,
      addCiMessage: true,
      addVoteOnMergeRequest: false,
      acceptMergeRequestOnSuccess: false,
      branchFilterType: 'All',
      cancelPendingBuildsOnUpdate: true,
      secretToken: env.API_TOKEN)
    }
    environment {
        CMAKE_PATH='cmake.exe'
        MSBUILD_PATH='"C:\\Program Files (x86)\\Microsoft Visual Studio\\2019\\Community\\MSBuild\\Current\\Bin\\MSBuild.exe"'
        CONFIGURATION='RelWithDebInfo'
    }
    stages {
        stage('configure') {
            steps {
                updateGitlabCommitStatus name: '1-configure', state: 'running'
                bat '%CMAKE_PATH% . -G "Visual Studio 16 2019" -A x64 -B build -DBUILD_IBR_ULR=ON -DBUILD_DOCUMENTATION=ON'
            }
            post {
                failure {
                    updateGitlabCommitStatus name: '1-configure', state: 'failed'
                }
                success {
                    updateGitlabCommitStatus name: '1-configure', state: 'success'
                }
            }
        }
        stage('build') {
            steps {
                updateGitlabCommitStatus name: '2-build', state: 'running'
                bat '%MSBUILD_PATH% build\\ALL_BUILD.vcxproj /p:Configuration=RelWithDebInfo'
                bat '%MSBUILD_PATH% build\\INSTALL.vcxproj /p:Configuration=RelWithDebInfo'
                bat '%MSBUILD_PATH% build\\docs\\doxygen\\compileDocs.vcxproj'
            }
            post {
                failure {
                    updateGitlabCommitStatus name: '2-build', state: 'failed'
                }
                success {
                    updateGitlabCommitStatus name: '2-build', state: 'success'
                }
            }
        }
        stage('test') {
            steps {
                updateGitlabCommitStatus name: '3-test', state: 'running'
                bat 'echo no test defined...'
            }
            post {
                failure {
                    updateGitlabCommitStatus name: '3-test', state: 'failed'
                }
                success {
                    updateGitlabCommitStatus name: '3-test', state: 'success'
                }
            }
        }
        stage('deploy') {
            parallel {
                stage('deploy_docs') {
                    when {
                        branch "master"
                    }
                    steps {
                        updateGitlabCommitStatus name: '4-deploy', state: 'running'

                        bat 'if not exist "sibr_docs" mkdir sibr_docs'

                        dir('sibr_docs') {
                            git branch: 'master',
                                credentialsId: 'GitlabCredentials',
                                url: 'https://gitlab.inria.fr/sibr/docs.git'
                            bat 'del /Q public'
                            bat 'copy /Y ..\\docs\\doxygen\\CompiledDocs\\html\\* public'
                            bat 'git add -A && git commit -m "Update master"'
                            bat 'git push origin master'
                        }
                    }
                    post {
                        failure {
                            updateGitlabCommitStatus name: '4-deploy', state: 'failed'
                        }
                        success {
                            updateGitlabCommitStatus name: '4-deploy', state: 'success'
                        }
                    }
                }
                stage('skip_deploy') {
                    when {
                        not {
                            branch "master"
                        }
                    }
                    steps {
                        bat 'echo not deploying if not master'
                        updateGitlabCommitStatus name: '4-deploy', state: 'success'
                    }
                }
            }
        }
    }
}
