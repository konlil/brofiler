//
//  Shader.fsh
//  TestGame
//
//  Created by 袁金刚 on 17/3/22.
//  Copyright © 2017年 konlil. All rights reserved.
//

varying lowp vec4 colorVarying;

void main()
{
    gl_FragColor = colorVarying;
}
