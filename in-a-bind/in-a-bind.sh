#!/bin/sh
#
# in-a-bind.sh - Internet Audit of BIND
# Derek Callaway <decal@security-objectives.com>
# 8 Aug 2008
#
# :-| Up-to-date nameserver
# :-) Vulnerable nameserver
# :-D Vulnerable nameserver that performs recursive queries
# :-( Invalid response 

# Number of nameservers to scan
TOTAL=1000

echo "*%* in-a-bind v1.0 *%*"
echo "======================"
echo
echo ":-| patched :-) vuln :-D vuln+recursive queries :-( invalid response"
echo

if [ ! -s content.rdf.u8.gz ]
  then wget http://rdf.dmoz.org/rdf/content.rdf.u8.gz
fi

if [ ! -s dmoz-domains.txt ]
  then zgrep '="http://' content.rdf.u8.gz | sed 's|^.*="\(http://[^"]*\)".*$|\1|g' | awk -F '://' '{print $2}' | awk -F '/' '{print $1}' | egrep -i '[.][a-z]{3}$' | sed -r 's|^(([^\/]+?\.)*)([^\.]{4,})((\.[a-z]{1,4})*)$|\3\4|gi' | grep -v '.*[.].*[.]' | sort -u > dmoz-domains.txt
fi

LINES=`wc -l < dmoz-domains.txt`
awk 'BEGIN{srand()}{printf "%08d %s\n",int(rand()*'"$LINES"'),$0}' dmoz-domains.txt | sort | sed 's/.\{9\}//' > random-domains.txt

COUNT=0
VULNRECURS=0

cp random-domains.txt tmp-domains.txt

while true
  do for d in `cat tmp-domains.txt | head -n "$[ TOTAL - COUNT ]"`
    do echo -n "-*- $COUNT "

    ../porkbind -c ../porkbind.conf $d > $d.pb 2> /dev/null

    COUNT=$[ COUNT + 1 ]

    if [ ! "`grep -i 'vulnerable to' $d.pb`" ]
      then if [ "`egrep -i 'unable to|cannot parse|timed out' $d.pb`" ]
        then rm $d.pb
  	echo -n ':-('
	COUNT=$[ COUNT - 1 ]
      else
        echo -n ':-|'
      fi
    else
      if [ "`grep -il 'recursive quer' $d.pb`" ]
        then VULNRECURS=$[ VULNRECURS + 1 ]
	echo -n ":-D"
      else
        echo -n ':-)'
      fi
    fi

    echo " $d"
  done

  tail -n +"$[ 1 + TOTAL ]" tmp-domains.txt > tmp-domains-new.txt
  mv tmp-domains-new.txt tmp-domains.txt

  [ $COUNT -ge $TOTAL ] && break
done

rm tmp-domains.txt

RECURSIVE=`grep -il 'recursive quer' *.pb | wc -l`
TRANSFERS=`grep -il 'zone transfer' *.pb | wc -l`
OUTDATED=`egrep -il 'CVE-200[^8]|CVE-[^2]' *.pb | wc -l`
UNPATCHED=`grep -il 'CVE-2008-1447' *.pb | wc -l`
DINOSAURS=`egrep -il 'CVE-200[21]|CVE-1' *.pb | wc -l`

UPTODATE=0
for d in *.pb
  do if [ ! "`egrep 'CVE-[0-9]{4}-[0-9]+' $d`" ]
    then UPTODATE=$[ UPTODATE + 1 ]
  fi
done

OUTPUTDIR=`date +'%d-%b-%Y-%H:%M'`

mkdir $OUTPUTDIR
mv *.pb $OUTPUTDIR

echo
echo "* $UNPATCHED domains were vulnerable"
echo "  - Un-Patched: $[ UNPATCHED - OUTDATED ]"
echo "  - Out-Dated: $[ OUTDATED - DINOSAURS ]"
echo "  - Dinosaurs: $DINOSAURS"
echo
echo "* $RECURSIVE domains had a nameserver that performed recursive queries
echo "  - Of these, $VULNRECURS were vulnerable to cache poisoning
echo
echo "* $TRANSFERS domains had a nameserver that performed zone transfers"
echo
echo "* $UPTODATE domains were Up-To-Date"
echo

exit 0
