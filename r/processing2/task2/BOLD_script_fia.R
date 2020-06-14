TIME_SERIES_ROOT_DIR <- "/project/4180000.18/GnG_awake/processing2/task2/"
TXT_ROOT_DIR <- "/project/4180000.18/GnG_awake/fls_3-column_file/bids2/"

writeToCsv <- function(subject, session, run, event) {

	## Read the timseries (.txt) as a table in R
	timeSeriesFileName <- sprintf("%ssub-%s/ses-%s/sub-%s_ses-%s_task-gng_acq-EPI_run-%s_bold/sub-%s_ses-%s_run-%s_timeseries.txt",
      	TIME_SERIES_ROOT_DIR, subject, session, subject, session, run, subject, session, run)
    print(sprintf("Timeseries file name is in %s", timeSeriesFileName))
	timeseries <- read.table(timeSeriesFileName, quote="\"", comment.char="")

	## Create an extra column in the beginning to indicate time in seconds
	time_in_seconds <- c((1:240)*1.5)

	## Extract regions of interest from the timeseries and average the area of interest (left and right)
	prelimbic_area <- ((timeseries[ ,c(16)] + timeseries[ ,c(52)])/2)
	agranular_insular_area <- ((timeseries[ ,c(19)] + timeseries[ ,c(55)])/2)
	caudoputamen <- ((timeseries[ ,c(28)] + timeseries[ ,c(64)])/2)

	## Combine the time and the averaged regions of interest
	regions_of_interest <- cbind(time_in_seconds,prelimbic_area,agranular_insular_area,caudoputamen)

	## Create the column names
	colnames(regions_of_interest) <- c("Time in seconds","Prelimbic area","Agranular insular area","Caudoputamen")

	## Read the glm.design (.txt) as a table in R
	txtFileName <- sprintf("%ssub-%s/ses-%s/sub-%s_ses-%s_run_%s_%s.txt", TXT_ROOT_DIR,
		subject, session, subject, session, run, event)
    print(sprintf("Txt file name is in %s", txtFileName))
	design <- read.table(txtFileName, quote="\"", comment.char="")

	## Create the column names
	colnames(design) <- c("Time in seconds","Duration of event","Standard magnitude")

	## Merge the matching timeseries from regions_of_interest and glm.design
	during_event <- merge(regions_of_interest, design, by = "Time in seconds")
	during_event <- during_event[ ,c(1:4)]
	during_event <- t(during_event)

	## Now do this for the upper and lower bound values
	lower_bound_first_values <- matrix(during_event[c(1), ]-1.5)
	colnames(lower_bound_first_values) <- c("Time in seconds")
	before_event_1 <- merge(regions_of_interest,lower_bound_first_values, by = "Time in seconds")
	before_event_1 <- t(before_event_1)

	lower_bound_second_values <- matrix(before_event_1[c(1), ]-1.5)
	colnames(lower_bound_second_values) <- c("Time in seconds")
	before_event_2 <- merge(regions_of_interest,lower_bound_second_values, by = "Time in seconds")
	before_event_2 <- t(before_event_2)

	upper_bound_first_values <- matrix(during_event[c(1), ]+1.5)
	colnames(upper_bound_first_values) <- c("Time in seconds")
	after_event_1 <- merge(regions_of_interest,upper_bound_first_values, by = "Time in seconds")
	after_event_1 <- t(after_event_1)

	upper_bound_second_values <- matrix(after_event_1[c(1), ]+1.5)
	colnames(upper_bound_second_values) <- c("Time in seconds")
	after_event_2 <- merge(regions_of_interest,upper_bound_second_values, by = "Time in seconds")
	after_event_2 <- t(after_event_2)

	## Extract the signal only from the prelimbic area

	first_event <- cbind((before_event_2[2,1]),(before_event_1[2,1]),(during_event[2,1]),(after_event_1[2,1]),(after_event_2[2,1]))
	second_event <- cbind((before_event_2[2,2]),(before_event_1[2,2]),(during_event[2,2]),(after_event_1[2,2]),(after_event_2[2,2]))
	third_event <- cbind((before_event_2[2,3]),(before_event_1[2,3]),(during_event[2,3]),(after_event_1[2,3]),(after_event_2[2,3]))
	hits <- rbind(first_event,second_event,third_event)
	rownames(hits) <- c("event 1", "event 2", "event 3")

	## Export the data frame to a .csv file
	setwd(TIME_SERIES_ROOT_DIR)
	outputFileName <- sprintf("sub-%s_ses-%s_run-%s_%s.csv", subject, session, run, event)
	print(sprintf("Output file is '%s'", outputFileName))
	write.table(hits, file = outputFileName, sep = ",", row.names = FALSE, col.names = FALSE)
}

printOnly <- function(subject, session, run, event) {
  timeSeriesFileName <- sprintf("%ssub-%s/ses-%s/sub-%s_ses-%s_task-gng_acq-EPI_run-%s_bold/sub-%s_ses-%s_run-%s_timeseries.txt",
	TIME_SERIES_ROOT_DIR, subject, session, subject, session, run, subject, session, run)
  print(sprintf("Timeseries file name is in %s", timeSeriesFileName))

  txtFileName <- sprintf("%ssub-%s/ses-%s/sub-%s_ses-%s_run_%s_%s.txt", TXT_ROOT_DIR,     subject, session, subject, session, run, event)
  print(sprintf("Txt file name is in %s", txtFileName))

  outputFileName <- sprintf("sub-%s_ses-%s_run-%s_%s.csv", subject, session, run, event)
  print(sprintf("Output file is '%s'", outputFileName))
}

for (subject in 3:3) {
	for (session in 1:1) {
		for (run in 1:5) {
			for (event in 1:4) {
				printOnly(subject, session, run, event)
				#writeToCsv(subject, session, run, event)
			}
		}
	}
}
