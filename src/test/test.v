/* example */
module test ( en, di, do, a, b);
input en, di, a, b;
output do;
reg tmp;
wire w;
always @() begin
	if (en) begin
		tmp <= di;
	end
	else begin
		tmp <= a + b;
	end
end
assign do = tmp;
endmodule;