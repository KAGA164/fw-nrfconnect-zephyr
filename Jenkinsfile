pipeline {
  agent any
  options {
    // Checkout the repository to this folder instead of root
    checkoutToSubdirectory('zephyr') 
  }
  parameters {
      string(name: 'PLATFORM', defaultValue: 'nrf52_pca10040', description: 'HW test board')
      string(name: 'FILTERS', defaultValue: '-T tests/kernel', description: 'Sanitycheck filter')
  }
  environment {
      // ENVs for sanitycheck
      ARCH = "-a arm"
      SANITYCHECK_OPTIONS = "--inline-logs --enable-coverage -N --device-testing"
      SANITYCHECK_RETRY = "--only-failed --outdir=out-2nd-pass"
      SANITYCHECK_RETRY_2 = "--only-failed --outdir=out-3rd-pass"

      // ENVs for building (triggered by sanitycheck)
      ZEPHYR_TOOLCHAIN_VARIANT = 'zephyr'
      GNUARMEMB_TOOLCHAIN_PATH = '/workdir/gcc-arm-none-eabi-7-2018-q2-update'
      ZEPHYR_SDK_INSTALL_DIR = '/opt/zephyr-sdk'
  }
  
  stages {
      stage('Repository checkout') {
         steps {
             // Initialize west
             sh "west init -l zephyr/"
             
             // Checkout
             sh "west update"
         }
      }
      
      stage('HW testing') {
        steps {          
          dir ("zephyr") {
            sh "echo variant: $ZEPHYR_TOOLCHAIN_VARIANT"
            sh "echo SDK dir: $ZEPHYR_SDK_INSTALL_DIR"
            sh "cat /opt/zephyr-sdk/sdk_version"
            
            script {
                SANITYCHECK_SERIAL_PORT = sh(returnStdout: true, 
                                          script: "python3 ./scripts/on_hw/platform_port_check.py -p ${params.PLATFORM}").trim()
            }

            sh "source zephyr-env.sh && \
                (./scripts/sanitycheck $SANITYCHECK_OPTIONS ${params.FILTERS} --device-serial $SANITYCHECK_SERIAL_PORT -p ${params.PLATFORM} $ARCH)" 
          }
        }        
      }
  }

  post {
    always {
      // Clean up the working space at the end (including tracked files)
      echo "End of post"
      cleanWs()
    }
  }
}
