proman_proto = Proto("proman","pRoman Protocol")

function proman_proto.dissector(buffer, pinfo, tree)
    pinfo.cols.protocol = "PROMAN"
    local subtree = tree:add(proman_proto, buffer(), "PRoman Protocol")

    local offset = 0

	-- cut-off packet
    if buffer:len() ~= buffer:reported_len() then
		return
	end

	local cmd_length = buffer(offset,4)
	subtree:add(buffer(offset, 4), "Command length: "..cmd_length:le_uint())
	offset = offset + 4

	if buffer:len() < offset + cmd_length:le_uint() then
		pinfo.desegment_len = cmd_length:le_uint()
		return
	end
	
	local command = buffer(offset,cmd_length:le_uint())
	subtree:add(buffer(offset,cmd_length:le_uint()), "Command: "..command:string())
	offset = offset + cmd_length:le_uint()

	if command:string() ~= "UPLOAD_IMAGE" and command:string() ~= "DELETE_IMAGE" and command:string() ~= "UPLOAD_IMAGE_WORKER" 
		and command:string() ~= "DELETE_IMAGE_WORKER" and command:string() ~= "RUN_NOW" and command:string() ~= "STOP_NOW" then
		return
	end

	if buffer:len() < offset + 4 then
		pinfo.desegment_len = 4
		return
	end

	local image_name_length = buffer(offset,4)
	subtree:add(buffer(offset, 4), "Image name length: "..image_name_length:le_uint())
	offset = offset + 4

	if buffer:len() < offset + image_name_length:le_uint() then
		pinfo.desegment_len = image_name_length:le_uint()
		return
	end

	local image_name = buffer(offset,image_name_length:le_uint())
	subtree:add(buffer(offset,image_name_length:le_uint()), "Image name: "..image_name:string())
	offset = offset + image_name_length:le_uint()

	if command:string() ~= "UPLOAD_IMAGE_WORKER" and command:string() ~= "DELETE_IMAGE_WORKER" 
		and command:string() ~= "RUN_NOW" and command:string() ~= "STOP_NOW" then
		return
	end

	if buffer:len() < offset + 4 then
		pinfo.desegment_len = 4
		return
	end

	local worker_id_length = buffer(offset,4)
	subtree:add(buffer(offset, 4), "Worker identifier length: "..worker_id_length:le_uint())
	offset = offset + 4

	if buffer:len() < offset + worker_id_length:le_uint() then
		pinfo.desegment_len = worker_id_length:le_uint()
		return
	end

	local worker_id = buffer(offset,worker_id_length:le_uint())
	subtree:add(buffer(offset,worker_id_length:le_uint()), "Worker identifier: "..worker_id:string())
	offset = offset + worker_id_length:le_uint()
	
end

tcp_table = DissectorTable.get("tcp.port")
tcp_table:add(1100, proman_proto)