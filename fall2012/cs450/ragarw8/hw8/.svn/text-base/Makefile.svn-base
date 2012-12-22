report:
	@touch REPORT.txt #Create an empty file if it does not exist
	@rm REPORT.txt
	@echo "Here is my report" >> REPORT.txt
	@echo "1. Following are the 30 AS:es that are the most frequently occurring ASNs between our hosts:" >> REPORT.txt
	@cat data/answer1|tr '\n' ', '|sed -e "s/,/, /g"|sed -e 's/,\s*$$/\.\n/g' >> REPORT.txt
	@echo "\n2. Following are the 30 AS:es that have the largest 'degree' or 'number of neighboring ASNs':" >> REPORT.txt
	@cat data/answer2|tr '\n' ', '|sed -e "s/,/, /g"|sed -e 's/,\s*$$/\.\n/g' >> REPORT.txt
	@echo "\n3. Following are the 30 AS:es that have the largest number of observed hosts:" >> REPORT.txt
	@cat data/answer3|tr '\n' ', '|sed -e "s/,/, /g"|sed -e 's/,\s*$$/\.\n/g' >> REPORT.txt
	@echo "\n4. Following are the AS:es that occur in all three lists above:" >> REPORT.txt
	@cat data/answer4|tr '\n' ', '|sed -e "s/,/, /g"|sed -e 's/,\s*$$/\.\n/g' >> REPORT.txt
	@echo "These AS:es happen to belong to Cogent Communications(174) , Level 3 communications(3356)  and  RIPE(20965)" >> REPORT.txt
	@echo "Ref:http://bgp.he.net/AS20965#_whois" >> REPORT.txt
	@cat REPORT.txt

all: collect postprocess report

postprocess:
	@echo "Preparing the data"	
	scripts/post-process

collect:
	scripts/collect

clean:
	rm -f logs/* *.log &
	scripts/clean_remote 
	wait
