proman_proto = Proto("proman","pRoman Protocol")

function proman_proto.dissector(buffer, pinfo, tree)
    pinfo.cols.protocol = "PROMAN"
    local subtree = tree:add(proman_proto, buffer(), "PRoman Protocol")

	local function get_next_arg(line)
		local f_space_index = string.find(line, " ")
		if f_space_index then
			return string.sub(line, 1, f_space_index - 1), string.sub(line, f_space_index + 1)
		end
		return line, nil
	end

	local function add_sub_arg(line, offset, name)
		arg, line = get_next_arg(line)
		subtree:add(buffer(offset + 1, arg:len()), name..arg)
		offset = offset + 1 + arg:len()
		return line, offset
	end

	local get_next_line = buffer():string():gmatch('[^\n]+')

	while true do
		local line = get_next_line()

		if line == nil then
			return
		end

		local arg, line = get_next_arg(line)
		local offset = 0
	
		if arg == nil then
			return
		end

		if arg == 'GET_WORKERS' then
			subtree:add(buffer(offset, arg:len()), "Command: 'GET_WORKERS'")		
		
		elseif arg == 'GET_IMAGES_LIST' then
			subtree:add(buffer(offset, arg:len()), "Command: 'GET_IMAGES_LIST'")

		elseif arg == 'UPLOAD_IMAGE' then
			subtree:add(buffer(offset, arg:len()), "Command: 'UPLOAD_IMAGE'")
			line, offset = add_sub_arg(line, offset, "Image Name: ")

		elseif arg == 'DELETE_IMAGE' then
			subtree:add(buffer(offset, arg:len()), "Command: 'DELETE_IMAGE'")
			line, offset = add_sub_arg(line, offset, "Image Name: ")

		elseif arg == 'UPLOAD_IMAGE_WORKER' then
			subtree:add(buffer(offset, arg:len()), "Command: 'UPLOAD_IMAGE_WORKER'")
			line, offset = add_sub_arg(line, offset, "Image Name: ")
			line, offset = add_sub_arg(line, offset, "Worker ID: ")
		
		elseif arg == 'DELETE_IMAGE_WORKER' then
			subtree:add(buffer(offset, arg:len()), "Command: 'DELETE_IMAGE_WORKER'")
			line, offset = add_sub_arg(line, offset, "Image Name: ")
			line, offset = add_sub_arg(line, offset, "Worker ID: ")

		else
			return
		end
	end
end

tcp_table = DissectorTable.get("tcp.port")
tcp_table:add(1100, proman_proto)