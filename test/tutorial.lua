local co_await = require 'lua_co_await'
local instance = co_await.create()
instance:start(4)
instance:run_tutorials()
