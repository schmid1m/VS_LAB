-- trivial protocol example
local MODES = {[0x01] = "Status Script", [0x02] = "Server", [0x03] = "Client"}
local FUNCTIONS = {[0x00] = "Generator Polynome", [0x01] = "Decrypt", [0x02] = "Unlock", [0x05] = "Broadcast", [0x06] = "Status"}
local TYPES = {[0x03] = "Request", [0x04] = "Response", [0x0F] = "Error"}

-- declare our protocol
vs_proto = Proto("vs","VS_LAB")

local fields	   = vs_proto.fields
fields.prio 	   = ProtoField.uint8 ("fields.prio"		, "            Priority "	, base.DEC)
fields.vers 	   = ProtoField.uint8 ("fields.vers"		, "            Version  "	, base.DEC)
fields.mode 	   = ProtoField.uint8 ("fields.mode"		, "            Mode     "	, base.DEC, MODES)
fields.fnc  	   = ProtoField.uint8 ("fields.fnc"			, "Function "				, base.DEC, FUNCTIONS	, 0xF0)
fields.msg_type    = ProtoField.uint8 ("fields.type"		, "Type     "				, base.DEC, TYPES		, 0x0F)
fields.len  	   = ProtoField.uint16("fields.len"			, "            Length   "	, base.DEC)
fields.res  	   = ProtoField.uint16("fields.res"			, "            Reserved "	, base.DEC)
fields.cid  	   = ProtoField.int16 ("fields.cid"			, "Client ID       "		, base.DEC)
fields.bid  	   = ProtoField.uint16("fields.bid"			, "Block ID        "		, base.DEC)
fields.gp   	   = ProtoField.uint16("fields.gp"			, "Generator       "		, base.HEX)
fields.crypt_dat   = ProtoField.uint16("fields.crypt_dat"	, "Encrypted data  "		, base.HEX)
fields.decrypt_dat = ProtoField.uint8 ("fields.crypt_dat"	, "Decrypted data  "		, base.HEX)
fields.seq_num     = ProtoField.uint32("fields.seq_num"	    , "Sequence Number "		, base.DEC)
fields.err         = ProtoField.uint8 ("fields.err"	        , "Error Code      "		, base.DEC)

-- create a function to dissect it
function vs_proto.dissector(buffer,pinfo,tree)
    pinfo.cols.protocol = "VS_LAB"

    local len_buf =buffer(4,2)
    local len = len_buf:uint()
    
    local subtree = tree:add(vs_proto,buffer(),"VS_LAB Protocoll Data")
    subtreeh = subtree:add(buffer(0,8),"Header")
    subtreeh:add(fields.prio, buffer(0,1))
    subtreeh:add(fields.vers, buffer(1,1))
    subtreeh:add(fields.mode, buffer(2,1))
    subtreeh:add(fields.fnc , buffer(3,1))
    subtreeh:add(fields.msg_type, buffer(3,1))
    subtreeh:add(fields.len, buffer(4,2))
    subtreeh:add(fields.res, buffer(6,2))
    subtreed = subtree:add(buffer(8,len),"Data")
    
    local fnc_buf = buffer(3,1)
    local fnc = fnc_buf:uint()
    fnc = bit.rshift(fnc, 4)
    
    local msg_type = fnc_buf:uint()
    msg_type = bit.band(msg_type, 0x0F)
    
    if((fnc == 0x00) and (msg_type == 0x03)) then
        subtreed:add(fields.cid, buffer(8,2))
        subtreed:add(fields.gp , buffer(10,2))
   	elseif((fnc == 0x02) and (msg_type == 0x03)) then
   		subtreed:add(fields.cid, buffer(8,2))
   	elseif((fnc == 0x01) and (msg_type == 0x03)) then
   		subtreed:add(fields.cid, buffer(8,2))
   		subtreed:add(fields.bid, buffer(10,2))
   		subtreedd = subtreed:add(buffer(12,len-4),"Encrypted Data")
   		for i=0,(len - 5),2 do
	   		subtreedd:add(fields.crypt_dat, buffer(12 + i,2))
	   	end
   	elseif((fnc == 0x01) and (msg_type == 0x04)) then
   		subtreed:add(fields.cid, buffer(8,2))
   		subtreed:add(fields.bid, buffer(10,2))
   		subtreedd = subtreed:add(buffer(12,len-4),"Decrypted Data")
   		for i=0,(len - 5) do
	   		subtreedd:add(fields.decrypt_dat, buffer(12 + i,1))
	   	end
   	elseif((fnc == 0x06) and (msg_type == 0x04)) then
   		subtreed:add(fields.cid, buffer(8,2))
   		subtreed:add(fields.res, buffer(10,2))
   		subtreed:add(fields.seq_num, buffer(12,4))
	elseif(msg_type == 0x0F) then
        subtreed:add(fields.err, buffer(8,2))
        subtreed:add(fields.bid , buffer(10,2))
   end
end
-- load the udp.port table
udp_table = DissectorTable.get("udp.port")
-- register our protocol to handle udp port 11111
udp_table:add(11111,vs_proto)
