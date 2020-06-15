TIME_SERIES_ROOT_DIR <- "C:\\\\Users\\User\\Documents\\Projects\\ArduinoMiceProject\\r\\processing2\\task2\\"
TXT_ROOT_DIR <- "C:\\\\Users\\User\\Documents\\Projects\\ArduinoMiceProject\\r\\bids-2\\"

writeToCsv <- function(subject, session, run, event) {

	## Read the glm.design (.txt) as a table in R
	txtFileName <- sprintf("%ssub-%s\\ses-%s\\sub-%s_ses-%s_run_%s_%s.txt", TXT_ROOT_DIR,
		subject, session, subject, session, run, event)

	# Read the timseries (.txt) as a table in R
	timeSeriesFileName <- sprintf("%ssub-%s\\ses-%s\\sub-%s_ses-%s_task-gng_acq-EPI_run-%s_bold\\sub-%s_ses-%s_run-%s_timeseries.txt",
		TIME_SERIES_ROOT_DIR, subject, session, subject, session, run, subject, session, run)

	if (file.exists(timeSeriesFileName) && file.exists(txtFileName)) {
		print(sprintf("Txt file name is in %s", txtFileName))
	    print(sprintf("Timeseries file name is in %s", timeSeriesFileName))

		final_table <- readData(txtFileName, timeSeriesFileName)

		## Export the data frame to a .csv file
		# outputFileName <- sprintf("%ssub-%s_ses-%s_run-%s_%s_prelimbic_area.csv", TIME_SERIES_ROOT_DIR, subject, session, run, event)
		# print(sprintf("Output file is '%s'", outputFileName))
		# write.table(final_table, file = outputFileName, sep = ",", row.names = FALSE, col.names = FALSE)

		final_table
	} else {
		c()
	}
}

readData <- function(txtFileName, timeSeriesFileName) {

	design <- read.table(txtFileName, quote="\"", comment.char="")
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

	## Extract the signals only from the prelimbic area and average the signals across all events
	mean_before_event_2 <- matrix(mean(before_event_2[c(2), ]))
	mean_before_event_1 <- matrix(mean(before_event_1[c(2), ]))
	mean_during_event <- matrix(mean(during_event[c(2), ]))
	mean_after_event_1 <- matrix(mean(after_event_1[c(2), ]))
	mean_after_event_2 <- matrix(mean(after_event_2[c(2), ]))

	final_table <- cbind(mean_before_event_2, mean_before_event_1, mean_during_event, mean_after_event_1, mean_after_event_2)
	final_table
}

printOnly <- function(subject, session, run, event) {

	## Read the glm.design (.txt) as a table in R
	txtFileName <- sprintf("%ssub-%s\\ses-%s\\sub-%s_ses-%s_run_%s_%s.txt", TXT_ROOT_DIR,
		subject, session, subject, session, run, event)
	if (file.exists(timeSeriesFileName)) {
		print(sprintf("Txt file name is in %s", txtFileName))
	} else {
		print(sprintf("Txt file name is in %s, but does not exist. So skipping...", txtFileName))
	}

	# Read the timseries (.txt) as a table in R
	timeSeriesFileName <- sprintf("%ssub-%s\\ses-%s\\sub-%s_ses-%s_task-gng_acq-EPI_run-%s_bold\\sub-%s_ses-%s_run-%s_timeseries.txt",
		TIME_SERIES_ROOT_DIR, subject, session, subject, session, run, subject, session, run)
	if (file.exists(timeSeriesFileName)) {
		print(sprintf("Timeseries file name is in %s", timeSeriesFileName))
	} else {
		print(sprintf("Timeseries file name is %s, but does not exist. So skipping...", timeSeriesFileName))
	}

	outputFileName <- sprintf("sub-%s_ses-%s_run-%s_%s_prelimbic_area.csv", subject, session, run, event)
	print(sprintf("Output file is '%s'", outputFileName))
}


for (event in 1:4) {

	outputFileName <- sprintf("%s_prelimbic_area.csv", event)
	print(sprintf("Output file is '%s'", outputFileName))

	for (subject in 3:7) {
		for (session in 1:1) {
			for (run in 1:9) {
				newData <- writeToCsv(subject, session, run, event)
				if (length(newData) > 0 && !is.na(newData)) {
					write.table(newData, file = outputFileName, sep = ",", row.names = FALSE, col.names = FALSE, append=TRUE)
				}
			}
		}
	}
}
