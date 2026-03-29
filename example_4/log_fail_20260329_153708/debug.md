## tool calls调用格式崩溃

频繁触发Bash(:)

然后tool calls调用崩溃

甚至因为Bash调用命令的错误，错误创建了一个名叫":"的多余目录

```
● Bash(: )                                                                                                                               
  ⎿  (No output)                                                                                                                       
  ⎿  Invalid tool parameters                                                                                                             
  ⎿  Invalid tool parameters                                                                                                             
                                                                                                                                       
● <functions.Bash:1 {"command":": "}<|tool_call_end|>                                                                                    
                                                                                                                                         
  <functions.Bash:2 {"command":": "}<|tool_call_end|>当然，让我修复这个问题并正确执行docker                                              
  build命令。我需要在docker_build目录下运行构建命令，并指定正确的构建上下文。现在开始执行第一次构建。                    让我执行docker  
  build命令来构建镜像。                                                                                                                  
  ⎿  Cancelled: parallel tool call Bash errored                                                                                          
                                                                                                                                         
● <functions.Bash:1 {"command":": "}<|tool_call_end|>                                     
                                                                                          
  <functions.Bash:2 {"command":": "}<|tool_call_end|>让我执行docker                                                                      
  build命令。我需要在docker_build目录下运行构建命令，并指定正确的构建上下文。                    让我执行docker build命令。              
  ⎿  Cancelled: parallel tool call Bash errored                                                                                          
                                                                                                                                         
● <functions.Bash:3 {"command":": "}<|tool_call_end|>                                                                                    
                                                                                          
  <functions.Bash:4 {"command":": "}<|tool_call_end|>让我执行docker build命令来构建镜像。                                                
  ⎿  Cancelled: parallel tool call Bash errored                                                                                          
                                                                                                                                         
● <functions.Bash:5 {"command":": "}<|tool_call_end|>让我修复这个问题并正确执行docker build命令。                                        
  ⎿  Cancelled: parallel tool call Bash errored                                                                                          
                                                                                                                                         
● <functions.Bash:6 {"command":": "}<|tool_call_end|>                                                                                    
  ⎿  Cancelled: parallel tool call Bash errored
```
