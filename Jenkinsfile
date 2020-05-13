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
                dir('build') {
                    bat '%CMAKE_PATH% --build . --target ALL_BUILD --config RelWithDebInfo'
                    bat '%CMAKE_PATH% --build . --target INSTALL --config RelWithDebInfo'

                    dir('docs/doxygen') {
                        bat '%CMAKE_PATH% --build . --target compileDocs'
                    }
                }
            }
            post {
                failure {
                    updateGitlabCommitStatus name: '2-build', state: 'failed'
                }
                success {
                    updateGitlabCommitStatus name: '2-build', state: 'success'
                    archiveArtifacts(artifacts: 'build/**, install/**, extlibs/**', fingerprint: true)
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
                stage('trigger_projects_build') {
                    when {
                        expression {
                            return gitlabBranch == "master"
                        }
                    }
                    steps {
                        updateGitlabCommitStatus name: '4-deploy', state: 'running'

                        // trigger sibr projects build
                        bat 'echo building projects and documentation'
                        build 'sibr_projects/master'
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
                        expression {
                            return gitlabBranch != "master"
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
