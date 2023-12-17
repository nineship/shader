## create a new repository on the command line
echo "# glshow" >> README.md  
git init  
git add README.md  
git commit -m "first commit"  
git branch -M main  
git remote add origin https  
git push -u origin main  

## push an existing repository from the command line
git remote add origin https  
git branch -M main  
git push -u origin main  

test  # game3d
# glshow
# glgame

该项目为opengl游戏贴图实现的非实时光影版本，  
直接利用阴影贴图来实现贴图，只需要写逻辑结构即可
不需要再渲染阴影。暂时搁置。
# gl_shadow
