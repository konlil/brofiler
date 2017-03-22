

# 设置包含路径
include_directories("${PROJECT_SOURCE_DIR}/BrofilerCore")
include_directories("${PROJECT_SOURCE_DIR}/BrofilerTest")

set(TARGET BrofilerIOSTest)

# 指定生成目标
add_executable(${TARGET} MACOSX_BUNDLE
				AppDelegate.h
				AppDelegate.mm
				GameViewController.h
				GameViewController.mm
				LaunchScreen.storyboard
				Shaders/Shader.fsh
				Shaders/Shader.vsh
				main.mm
			)

# 添加链接库
target_link_libraries(${TARGET} 
							BrofilerCore
							BrofilerTest)

target_link_libraries(${TARGET} "-framework Security" "-framework CoreGraphics" "-framework MobileCoreServices" "-framework SystemConfiguration" "-framework QuartzCore" "-framework MediaPlayer" "-framework MessageUI" "-framework CoreTelephony" "-framework CoreLocation" "-framework AVFoundation" "-framework GameController" "-framework CoreMotion" "-framework GLKit")


#设置xcode属性
set(_plist_in "${CMAKE_MODULE_PATH}/IOSBundleInfo.plist.in")
set_target_properties(${TARGET} PROPERTIES XCODE_ATTRIBUTE_CODE_SIGN_IDENTITY "iPhone Developer" MACOSX_BUNDLE_INFO_PLIST ${_plist_in})