

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

#拷贝自定义资源到app bundle
set(BUNDLE_RES_LIST Shaders/Shader.fsh Shaders/Shader.vsh)
ADD_CUSTOM_COMMAND(
			TARGET ${TARGET} 
			POST_BUILD
			COMMAND ${CMAKE_COMMAND} 
			  -DFILES_LIST="${BUNDLE_RES_LIST}"  -DDESTINATION="\${TARGET_BUILD_DIR}/\${FULL_PRODUCT_NAME}" 
			  -DEXCLUDE_EXT=".svn .git CVS .DS_Store" 
			  -P ${CMAKE_MODULE_PATH}/CopyFiles.cmake
			WORKING_DIRECTORY ${CMAKE_CURRENT_LIST_DIR}
COMMENT "Copying resource files to iOS App Bundle..." )

